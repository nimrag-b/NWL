# A Simplistic Language for HTML

I made this for a couple of reasons, the first being a general distain for most modern web technologies, and as well as wanting to build my own programming language, and make something that makes it really easy to produce dynamic webpages.

The syntax is (by design) very similar to C based languages. It is statically typed, since I dislike dynamic languages, and is currently a single pass parser.

At this stage, it is very simple, but supports a few inbuilt functions, such as getting the current date, printing to the console and doing arithmetic and string concaternation. It also supports textual includes, much like C, that can be used for templating, reusing code etc.

### DISCLAIMER

Would I reccommend this for any sort of real usage? In its current stage, absolutely not, even if it is *usable*. This is at best, a fun side project, that I am hoping will become a cool project eventually.

## Building

Simply run Make in the src directory, and it will produce a standalone file. It does not require any external libraries.

## Usage

Pass the name of the file you wish to process into the executable, and it will emit HTML to stdout.
