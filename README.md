# am-ui

# What is am-ui?

"AmLang" is a new programming language mainly targetted at niche platforms like AmigaOS (Also playing with MorphOS, Nintendo Wii Homebrew, Raspberry Pi Pico, Atari TOS). Read more about it below.

am-ui is a UI library that one could use when programming AmLang. It let's you open a Screen or a Window, add some layouts, labels, buttons etc. Work in progress.

# Am Lang Features
AmLang is inspired by other programming languages like Kotlin, Java, C#, TypeScript, Swift etc. 

- ARC (automatic reference counting)
- classes
- interfaces
- namespaces
- suspendable functions
- lambda expressions
- native c support, 
- exceptions (try,catch,throw) 
- and a lot more. 

There are still some major features missing, like for example for-loops. I have to use while loops until I prioritize for-loops 🙂 Currently it compiles only on Mac/Linux/Windows (using Docker), but one of the ultimate goals is to re-write the compiler in its own language. The compiler doesn't generate any machine code on its own, it writes C-code and lets GCC make the machine code. Performance-wise it's not as efficient as C (obviously?), but one can easily let c/asm do the heavy lifting and use this for orchestration. 

# Requirements
- Java 11+
- Docker
- Windows, MacOS, Linux

# Quick start (for AmigaOS3):

There is no "main" function in this library, but there are some examples in src/Am/Examples/Examples.aml - just rename for example main12() to main() and build.

build for AmigaOS (3.x): \
java -jar amlc-1.jar . -acmd -bt amigaos_docker_unix-x64

add more runtime logging: \
java -jar amlc-1.jar . -acmd -bt amigaos_docker_unix-x64 -rl

add more comments in the code: \
java -jar amlc-1.jar . -acmd -bt amigaos_docker_unix-x64 -rdc

Binary will end up in builds/bin/amigaos/ as a file called "app". Copy the "app" file to an Amiga environment and execute it. 

The option "-acmd" means that you allow the compiler to run commands from the package.yml file. 

The option "-bt" specifies which build target (from package.yml) you want to use (which platform to build for and how to build)

