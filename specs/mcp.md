# The language's MCP

The specification of [The language's MCP](https://github.com/Spite-Language/spite_documentation/blob/master/mcp.md):
every rule of this part of the language, with its edge cases and its exact error texts. The page teaches; this one
decides, and where they disagree, the page is the bug.

An AI drives a running program through the language's MCP: a [Model Context Protocol](https://modelcontextprotocol.io)
server, built into the compiler, whose tools run Spite in the program and answer what it gives. It is the AI's
counterpart of the REPL ([repl.md](repl.md)), which is a person's console: both run code through the same session
and the same swap machinery, and neither has commands. It is a development tool: it runs any code with no
authentication, so a production build never carries it. Windows only for now, like the swap machinery; Linux and
macOS are spite_compiler#297.

## Two halves

- **The program: `--mcp-port=<port>`.** `mcp_port` is a `Build` field ([Build settings: `Build`](programs.md#build-settings-build))
  holding a port, `0` (the default) for none, so the port is part of the build: there is no run-time override, and
  changing the port means rebuilding. It implies `--hot-reload` ([repl.md](repl.md#live-reload-in-detail)), the
  swap machinery and the file watcher, since the AI edits files and its next call expects them live; so the build is
  inspectable and never a production build ([compiler.md](compiler.md#inspectable-and-production-builds)). A port
  outside 1 to 65535 is `error: --mcp-port takes a port number from 1 to 65535: spite --mcp-port=4000`.
- **The server: `spite --mcp=<port>`.** The compiler binary run with `--mcp` is an MCP server over standard input and
  output (JSON-RPC 2.0, one message a line), which an MCP client starts as a command
  (`{"command": "spite", "args": ["--mcp=4000"]}`) from any folder. It serves one program, the one listening on
  `127.0.0.1:<port>`. Any other setting beside it is `error: '<setting>' is not a setting of --mcp, which takes the
  port of the program it serves: spite --mcp=4000`, and a port outside 1 to 65535 is `error: --mcp takes a port
  number from 1 to 65535: spite --mcp=4000`. It answers `initialize` with its name `spite` and the compiler's
  version, and `tools/list` with the tools below.

## The program's side

- **Loopback only, no authentication.** Before the entry constructor runs, `main` listens on `127.0.0.1:<port>`
  with `Tcp.Server.listen_locally` ([System classes](standard_library.md#system-classes)), which binds nothing else,
  so "loopback only" holds by construction; anyone who can reach that port on that machine can run any code in the
  program. A port another program holds stops the program before its constructor with `error: the MCP could not
  listen on 127.0.0.1:<port>` and exit code 1.
- **One connection at a time**, on a thread of the MCP's own: a two-line thread entry the compiler writes,
  `spite_mcp_thread`, started and waited for through the operating system's folder (`CreateThread` and
  `WaitForSingleObject` from `kernel32.dll`; `pthread_create` and `pthread_join` elsewhere). The thread runs no
  Spite code of the program: it reads a request, compiles it, and hands it to the program's thread as the console's
  thread hands a line ([Answered where the program waits](repl.md#answered-where-the-program-waits)), so a request
  sees the program between two steps and is answered at its waits and loop check points.
- **The program stays alive.** When the entry constructor returns, the process keeps serving until code it is sent
  calls `program.exit(code)`, which flushes what the program printed and ends it with that code. A client that
  disconnects ends nothing.
- **The wire between the two halves** is one request a line and one answer a line, JSON, and is the compiler's own:
  it is not a promise, and nothing but `spite --mcp` speaks it.

## The tools

- **`evaluate`**, input `{"code": string}`. It waits until the saves are settled ([Settled](repl.md#live-reload-in-detail)),
  then runs `code` in the program as a REPL line runs ([A line is Spite](repl.md#--repl)): an expression answers its
  value, a statement answers nothing, `program` names the entry instance, and `var` and `func` declare attributes and
  functions of the MCP's session, which later `evaluate` calls name. The program holds one session for the MCP, kept
  across connections and apart from a REPL's. The answer is a text content holding the value as `"{value}"` makes it
  text, and `structuredContent` `{"value": "...", "type": "<class>"}`, the type `Nothing` for a statement or a
  `Nothing` expression. It is never answered from code older than the saves: each of these is a tool error
  (`isError: true`) whose text is the message, and nothing runs:
  - a save whose reload is held, with the held message and the `reload` that settles it: `the reload of Hero is
    held, since it would lose data: level is gone and rank is new: reload with renames {Hero.attributes["level"]:
    "rank"} to keep what level holds, or with renames {} to let it go`;
  - saved files that do not compile, `the saved files do not compile, so the program still runs the code before
    them: <the compiler's error>`;
  - code that does not compile, with the compiler's error; the session stays as it was;
  - ten minutes without the saves settling, with the message in [Settled](repl.md#live-reload-in-detail).

  A crash in the code ends the program, as every crash does; the answer is a tool error, `the program ended: <its
  crash line>`.
- **`reload`**, input `{"renames": string}`, `renames` optional. It compiles the saved files now, as a `load` of
  every changed file would ([Loading a changed file](repl.md#loading-a-changed-file)), and answers what it rebuilt,
  moved or removed, or `no code the program runs changed, so nothing was rebuilt`. `renames` is the rename map of
  step 1 of [Nothing needs a restart](repl.md#nothing-needs-a-restart), written as the Spite map literal
  (`{Hero.attributes["level"]: "rank"}`); an entry that is not one is refused before anything is compiled, with the
  message that step gives. A refused reload is a tool error, and the program keeps all of its code.
- **Nothing listening.** A tool called while no program listens on the port answers a tool error, `nothing is
  listening on 127.0.0.1:<port>: start the program built with --mcp-port=<port>`, and the server keeps running, so
  the next call finds a program started meanwhile.

## The language's MCP and an application's

The language's MCP is a debugging tool, never shipped: it runs any code, with no authentication. A program that
lets an AI change it in production (a game server letting an AI tune its tick rate) writes its own tools, which
decide what an AI can touch: an application's MCP. A library for writing one is proposed in spite_standard_library#39 and
not specified yet.

---

Next: [Testing](testing.md).
