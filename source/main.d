import std.stdio;
import std.string;
import std.ascii : toLower, toUpper;
import std.conv;
    
class InvalidFormatException : Exception {
    this(string msg) {
        super(msg);
    }
}

class Question {
    string prompt;
    string[] answers;
    int reward;
    bool caseSensitive;

    // Throws: InvalidFormatException
    this(string line) {
        string[] splitLine = line.split(";");
        if (splitLine.length > 4) {
            throw new InvalidFormatException("Too many fields in Question line!");
        }

        this.prompt = splitLine[0].strip();
        this.answers = [];
        foreach (itm; splitLine[1].strip().split(",")) {
            // dynamic array appends are scuffed
            this.answers[this.answers.length++] = itm.strip;
        }

        try {
            this.reward = splitLine[2].strip().to!int(); 
        } catch (ConvException e) {
            throw new InvalidFormatException("Invalid field for reward: %s".format(e));
        }
        
        this.caseSensitive = false;
        if (splitLine.length == 4) {
            char c = splitLine[3].strip()[0].toLower();
            switch (c) {
                case 'y': {
                    this.caseSensitive = true;
                } break;
                case 'n': {
                    this.caseSensitive = false;
                } break;
                default:
                    throw new InvalidFormatException(
                        "Last field's case sensitivity specifier `%c` is invalid".format(c));
            }
        }
    }
    
    void ask() {
        writefln("question: \"%s\"", prompt);
        foreach (answer; answers) {
            writefln("ans: \"%s\"", answer);
        }
        writefln("reward: \"%s\"", reward);
        if (caseSensitive) {
            writeln("case sensitive");
        } 
    }
}

void main(string[] args) {
    string path;
    if (args.length <= 1) {
        write("enter file path: ");
        path = readln().strip();
    } else {
        path = args[1].strip();
    }

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
}
