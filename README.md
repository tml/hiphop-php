HipHop is a source code transformer which transforms PHP source code into highly optimized C++ and then compiles it using g++.

## Status

This is a placeholder file as we work towards releasing the HipHop source code.  We really wanted to have it out by now, but have run into some build/compilation issues when removing certain Facebook-specific extensions.

Within the next few days (and maybe even sooner) we'll release an initial copy of the source code that you can build on CentOS, but doesn't have a lot of commit history and has some workarounds for these build issues.

We'll then follow that up with a much cleaner source code release, commit history, commits as the team continues working, and be in a state to accept contributions back.  Overall we're erring a bit more on the side of making sure that you can get HipHop up and running without too much work.

The wiki containing the documentation is now available (see the wiki link above).  Also check out [http://www.ustream.tv/recorded/4409735](http://www.ustream.tv/recorded/4409735) for a recording of last week's tech talk.

Want to know more, take a look at the [thread](http://groups.google.com/group/hiphop-php-dev/browse_thread/thread/2babfd7c38ccea06) on the developer list.


## Why HipHop

One of the explicit design goals leading into HipHop was the ability to continue writing complex logic directly within PHP.  Companies with large PHP codebases will generally rewrite their complex functionality directly as PHP extensions in either C or C++.  Doing so ends up reducing the number of people who are able to work on the company's entire codebase.

HipHop is not the right solution for everyone deploying PHP.  We think it will be useful to companies running very large PHP infrastructures who do not wish to rewrite complex logic within C or C++.


## License

HipHop will be released under the open source PHP license.


## Discussion and support

You can discuss HipHop for PHP on the [HipHop developer mailing list](http://groups.google.com/group/hiphop-php-dev)

