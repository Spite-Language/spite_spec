# Notes

The source of this program is written with '\r' line breaks on purpose, and git keeps it so (`-text` in the repository's `.gitattributes`): the compiler reads it, rewrites every line break as '\n', and prints a `fixed:` line.
