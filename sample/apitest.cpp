#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "../include/UraniumVM.h"

// log runtime information
static uvm_callback_return_t interp_callback_log(uvm_callback_args_t *args) {
  FILE *logfp = (FILE *)args->usrctx;
  switch (args->op) {
    case vcop_read:
    case vcop_write: {
      break;
    }
    case vcop_call: {
      fprintf(logfp, "vcop call : func %p.\n", args->info.call.callee);
      break;
    }
    case vcop_return: {
      fprintf(logfp, "vcop return : hit address %p.\n", args->info.ret.hitaddr);
      break;
    }
#if __ARM__
    case vcop_svc: 
#else
    case vcop_syscall:
#endif
    {
      fprintf(logfp, "vcop syscall : syscall number %d.\n",
              args->info.svc.sysno);
      break;
    }
    case vcop_ifetch: {
      break;
    }
    default: {
      fprintf(logfp, "unknown vcop %d.\n", args->op);
      break;
    }
  }
  return cbret_directcall;
  // return cbret_recursive;
}

// do nothing
static uvm_callback_return_t interp_callback_nop(uvm_callback_args_t *args) {
  return cbret_directcall;
  // return cbret_recursive;
}

static int numbers[] = {2, 0, 2, 0, 2, 2, 0, 20, 20, 1, 199, 100000000};

// interpretee
static int print_message(const char *reason, const char **argv, FILE *cblogfp,
                         uvm_interp_callback_t cb) {
  free(malloc(1024 * 1024));

  std::vector<std::string> svec;
  std::set<std::string> sset;

  std::map<std::string, std::string> smap;
  std::vector<int> ivec;
  std::set<int> iset;
  std::map<int, int> imap;

  std::string hi("hello, ");
  hi += argv[0];
  std::cout << reason << " : " << hi << std::endl;
  printf("printf, %s %s %c %d %p %d\n", reason, argv[0], '$', (int)strlen(argv[0]),
         print_message, (int)hi.length());

  puts("numbers: ");
  for (int i = 0; i < sizeof(numbers) / sizeof(numbers[0]); i++) {
    char si[16], sv[16];
    sprintf(si, "%d", i);
    sprintf(sv, "%d", numbers[i]);
    svec.push_back(si);
    sset.insert(si);
  
    smap.insert(std::make_pair(si, sv));
    ivec.push_back(i);
    iset.insert(i);
    imap.insert(std::make_pair(i, numbers[i]));

    switch (numbers[i]) {
      case 0: {
        printf(" %d", numbers[i] + 1);
        break;
      }
      case 1: {
        printf(" %d", numbers[i] + 2);
        break;
      }
      default: {
        if (numbers[i] > 10) {
          printf(" %d", numbers[i]);
        } else {
          printf(" %d", numbers[i] + 3);
        }
        break;
      }
    }
  }
  putchar('\n');

  std::cout << "string vector : ";
  for (size_t i = 0; i < svec.size(); i++) {
    std::cout << svec[i] << " ";
  }
  putchar('\n');

  std::cout << "integer vector : ";
  for (auto i : ivec) {
    std::cout << i << " ";
  }
  putchar('\n');

  std::cout << "string set : " << *sset.find("3") << " ";
  for (auto &s : sset) {
    std::cout << s << " ";
  }
  putchar('\n');

  std::cout << "integer set : " << *iset.find(3) << " ";
  for (auto i : sset) {
    std::cout << i << " ";
  }
  putchar('\n');

  std::cout << "string map : " << smap.find("3")->second << " ";
  for (auto &s : smap) {
    std::cout << "<" << s.first << ", " << s.second << "> ";
  }
  putchar('\n');

  std::cout << "integer map : " << imap.find(3)->second << " ";
  for (auto &i : imap) {
    std::cout << "<" << i.first << ", " << i.second << "> ";
  }
  putchar('\n');

  fflush(stdout);
  return strlen(argv[0]);
}

// run interpretee directly
int vrun_print_message(const char *reason, const char **argv, FILE *cblogfp,
                       uvm_interp_callback_t cb) {
  uvm_context_t ctx;
  memset(&ctx, 0, sizeof(ctx));
  ctx.usrctx = cblogfp;
  ctx.callback = cb;
#if __ARM__
  ctx.uvmctx.r[0].p = reason;
  ctx.uvmctx.r[1].p = (void *)argv;
#else
#if __x64__
  ctx.uvmctx.rdi.p = (void *)reason;
  ctx.uvmctx.rsi.p = (void *)argv;
#else
  // x86 uses stack to pass parameter
  void *args[3];
  args[0] = (void *)vrun_print_message; // simulate call return address
  args[1] = (void *)reason;
  args[2] = (void *)argv;
  ctx.uvmctx.rsp.p = (void *)&args[0];
#endif
#endif
  return (int)(long)uvm_run_interp((void *)print_message, &ctx);
}

// run interpretee with a wrapper
int wrapper_print_message(const char *reason, const char **argv, FILE *cblogfp,
                          uvm_interp_callback_t cb) {
  const void *fnptr = uvm_make_callee((void *)print_message, cblogfp, cb);
  return ((int (*)(const char *, const char **))fnptr)(reason, argv);
}

int main(int argc, const char *argv[]) {
  printf("YunYoo UraniumVM test program, pid is %d.\n", getpid());
  FILE *cblogfp = fopen(
#if __APPLE__
    "/tmp/uvm.log", 
#else
    "/data/local/tmp/uvm.log", 
#endif
    "w");
  struct {
    const char *reason;
    int (*func)(const char *, const char **, FILE *cblogfp, uvm_interp_callback_t);
    uvm_interp_callback_t interpcb;
  } testor[] = {
      {
          "direct",
          print_message,
          nullptr,
      },
      {
          "vrun0",
          vrun_print_message,
          nullptr,
      },
      {
          "vrun1",
          vrun_print_message,
          interp_callback_nop,
      },
      {
          "wrapper0",
          wrapper_print_message,
          interp_callback_nop,
      },
      {
          "wrapper1",
          wrapper_print_message,
          interp_callback_log,
      },
  };
  for (int i = 0; i < sizeof(testor) / sizeof(testor[0]); i++) {
    printf("////// index %d //////\n", i);
    fflush(stdout);
    printf("////// result %d //////\n\n",
           testor[i].func(testor[i].reason, argv, cblogfp, testor[i].interpcb));
    fflush(stdout);
  }
  fclose(cblogfp);
  return 0;
}
