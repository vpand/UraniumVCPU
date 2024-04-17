ios_arm64:
	clang++ -target arm64-apple-ios10.0 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk -std=c++1z -Os -g -o ./ios/uraniumvm_apitest ./apitest.cpp ../ios/arm64/liburaniumvm.dylib

mac_arm64:
	clang++ -target arm64-apple-macos11.0 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk -std=c++1z -Os -g -o ./mac/uraniumvm_apitest_arm64 ./apitest.cpp ../mac/arm64/liburaniumvm.dylib

mac_x64:
	clang++ -target x86_64-apple-macos10.15 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk -std=c++1z -Os -g -o ./mac/uraniumvm_apitest_x64 ./apitest.cpp ../mac/x64/liburaniumvm.dylib
