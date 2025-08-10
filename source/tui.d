import std.stdio;
import std.format;
import std.conv;
import std.typecons : Nullable;
import std.sumtype;

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
}

private int getColorLength() {
    return Color.max+1 - Color.min;
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
    private E cur;
    private Color activeColor;
    string[] opts;

    import tui.Color;
    immutable OBR = S_DIM ~ "[" ~ S_END;
    immutable CBR = S_DIM ~ "]" ~ S_END;

    this(string[] opts, Color activeColor = GREEN) {
        static assert(getColorLength() == opts.length, "length of enum must match length of options!");

        this.activeColor = color;
        this.opts = opts;
        this.cur = cast(E)0;
    }

    private void getVariantString(Color c) {
        return opts[cast(int)c];
    }

    private string formatOneOption(String str) {
        return "%s%s%s%s%s".format(OBR, color.getColorEscapeCode(true), str, S_END, CBR);
    }

    void draw(File stream = stdout()) {
        foreach (opt; opts) {
            stream.write(formatOneOption(opt));
            stream.write(" ");
        }
        stream.flush();
    }

    Key getNextCommand() {
        toggleRawMode();
        scope(exit) toggleRawMode();

        char ch = cast(char)getchar();
        if (ch == '\033') {
            switch (getchar()) {
                case 'B': {
                    return Command.SELECT;
                } break;
                case 'C': {
                    return Command.RIGHT;
                } break;
                case 'D': {
                    return Command.LEFT;
                } break;
                default: return '\033';
            }
        }

        switch (ch) {
            case '\n': {
                return Command.SELECT;
            } break;
            default: return ch;
        }
    }

    // returns true if handled
    private void handleCommand(Command cmd) {
        immutable int MAX = E.max + 1 - E.min;
        switch (cmd) {
            case RIGHT: {
                cur = cast(E)((cast(int)cur + 1) % MAX);
            } break; 
            case LEFT: {
                cur = cast(E)((cast(int)cur - 1) % MAX);
            } break;
            default: break;
        }
    }

    private void handleKey(Key k) {
        return k.match!(
            (char _) => false, // no-op for now
            handleCommand,    
        );    
    }

    E run(File stream = stdout()) {
        stream.write(S_HIDE_CURSOR);
        Key key = Key.init;

        do {
            draw(stream);
            key = getNextCommand();
            if (key == Command.SELECT)
                break;
            handleKey(k);
        } while (true);

        stream.write(S_SHOW_CURSOR);
        return cur;
    }
}

void toggleRawMode() {
    termios raw = {};
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
