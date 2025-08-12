import std.stdio;
import std.format;
import std.conv;
import std.typecons;
import std.sumtype;
import std.algorithm.searching;

import core.sys.posix.termios;
import core.sys.posix.unistd;

immutable S_BOLD = "\033[1m";
immutable S_DIM  = "\033[2m";
immutable S_END  = "\033[0m";
immutable S_BLACK   = "\033[30m";
immutable S_RED     = "\033[31m";
immutable S_GREEN   = "\033[32m";
immutable S_YELLOW  = "\033[33m";
immutable S_BLUE    = "\033[34m";
immutable S_MAGENTA = "\033[35m";
immutable S_CYAN    = "\033[36m";
immutable S_WHITE   = "\033[37m";
immutable S_BG_BLACK   = "\033[40m";
immutable S_BG_RED     = "\033[41m";
immutable S_BG_GREEN   = "\033[42m";
immutable S_BG_YELLOW  = "\033[43m";
immutable S_BG_BLUE    = "\033[44m";
immutable S_BG_MAGENTA = "\033[45m";
immutable S_BG_CYAN    = "\033[46m";
immutable S_BG_WHITE   = "\033[47m";
immutable S_CLEAR_SCREEN = "\033[2J\033[H";
immutable S_CLEAR_LINE  = "\r\033[K";
immutable S_ENTER_ALT = "\033[?1049h";
immutable S_LEAVE_ALT = "\033[?1049l";
immutable S_SHOW_CURSOR = "\033[?25h";
immutable S_HIDE_CURSOR = "\033[?25l";

enum Color {
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
}

enum Command {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SELECT,
    NONE,
}

private int getEnumLength(E)() if (is(E == enum)) {
    return E.max+1 - E.min;
}

private string getColorEscapeCode(Color c, bool bold = false) {
    if (bold) {
        return "\033[1;%dm".format(cast(int)c);
    } else {
        return "\033[%dm".format(cast(int)c);
    }
}

alias EscapedChar = char;
alias Key = SumType!(Command, char);

class HorizontalMenu(E) if (is(E == enum)) {
    private int cur;
    private Color activeColor;
    string[] opts;
    bool showExit;

    immutable OBR = S_DIM ~ "[" ~ S_END;
    immutable CBR = S_DIM ~ "]" ~ S_END;

    this(string[] opts, bool showExit = true, Color activeColor = Color.GREEN) {
        assert(getEnumLength!E() == opts.length, "length of enum must match length of options!");

        this.activeColor = activeColor;
        this.opts = opts;
        this.cur = 0;
        this.showExit = showExit;
    }
    
    private E getCurEnum() {
        return cast(E)cur;
    }

    private string getVariantString(Color c) {
        return opts[cast(int)c];
    }

    private string formatExit() {
        if (!showExit) {
            return "";
        }

        if (cur == opts.length) {
            return OBR ~ getColorEscapeCode(Color.RED, true) ~ "exit" ~ S_END ~ CBR;
        } else {
            return OBR ~ "exit" ~ CBR;
        }
    }

    private string formatOneOption(int i) {
        if (i == opts.length && showExit)
            return formatExit();

        string str = opts[i];
        string cur_s = "";

        if (cur == opts.length && showExit) {
        } else {
            cur_s = opts[cur];
        }

        if (str == cur_s) {
            return OBR ~ activeColor.getColorEscapeCode(true) ~ str ~ S_END ~ CBR;
        } else  {
            return OBR ~ str ~ CBR;
        }
    }

    void draw(File stream = stdout()) {
        auto max = opts.length;
        if (showExit)
            max++;

        for (auto i = 0; i < max; i++) {
            stream.write(formatOneOption(i));
            stream.write(" ");
        }
    }

    Command getNextCommand() {
        char ch = rgetchar();
        if (ch == '[') {
            switch (rgetchar()) {
                case 'B': {
                    return Command.SELECT;
                } break;
                case 'C': {
                    return Command.RIGHT;
                } break;
                case 'D': {
                    return Command.LEFT;
                } break;
                default: return Command.NONE;
            }
        }

        switch (ch) {
            case '\n': {
                return Command.SELECT;
            } break;
            default: return Command.NONE;
        }
    }

    // returns true if handled
    private void handleCommand(Command cmd) {
        int max = getEnumLength!E;
        if (showExit)
            max++;

        switch (cmd) {
            case Command.RIGHT: {
                cur = (cur + 1) % max;
            } break; 
            case Command.LEFT: {
                // force modulus to be always positive
                int newval = cur - 1;
                cur = (newval % max + max) % max;
            } break;
            default: break;
        }
    }

    Nullable!E run(File stream = stdout()) {
        stream.write(S_HIDE_CURSOR);
        scope(exit) stream.write(S_SHOW_CURSOR);

        auto cmd = Command.init;
        do {
            draw(stream);
            cmd = getNextCommand();

            if (cmd == Command.SELECT)
                break;
            
            handleCommand(cmd);
            stream.write("\r");
        } while (true);

        if (cur < opts.length)
            return getCurEnum();
        else
            return Nullable!E.init;
    }
}

private char rgetchar() {
    toggleRawMode();
    char ch = cast(char)getchar();
    toggleRawMode();
    return ch;
}

void toggleRawMode() {
    termios raw = {};
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
