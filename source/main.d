import std.stdio;
import std.string;
import std.ascii : toLower, toUpper;
import std.conv;
import question;
import std.process;

import core.stdc.stdlib : exit;
import core.sys.posix.termios;
import core.sys.posix.unistd;
import argparse;
import tui;

immutable VERSION = "0.2.0";
termios origTermios = {};

struct Args {
    @PositionalArgument(0)
    string path;

    @NamedArgument("v", "version")
    void _version() {
        writeln("askthing version ", VERSION);
        exit(0);
    }
}

void init() {
    tcgetattr(STDIN_FILENO, &origTermios);
}

void deinit() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios);
}

int main(string[] argv) {
    // manually calling parseargs because this main is good enough
    Args args;
    if (!CLI!Args.parseArgs(args, argv[1..$]))
        return 1;

    string path = args.path;

    File f = File(path, "r");
    scope(exit) f.close();

    Question[] qns = [];
    string line;
    while ((line = f.readln().strip()) !is null) {
        if (line.length == 0)
            continue;
        
        if (line[0..2] == "//")
            continue;
        
        try {
            qns[qns.length++] = new Question(line);
        } catch (InvalidFormatException e) {
            stderr.writefln("failed to parse line: %s", e);
        }
    }

    foreach (q; qns) {
        q.ask();
    }
    
    return 0;
}
