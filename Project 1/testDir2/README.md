<style type="text/css">.rendered-markdown{font-size:14px} .rendered-markdown>*:first-child{margin-top:0!important} .rendered-markdown>*:last-child{margin-bottom:0!important} .rendered-markdown a{text-decoration:underline;color:#b75246} .rendered-markdown a:hover{color:#f36050} .rendered-markdown h1, .rendered-markdown h2, .rendered-markdown h3, .rendered-markdown h4, .rendered-markdown h5, .rendered-markdown h6{margin:24px 0 10px;padding:0;font-weight:bold;-webkit-font-smoothing:antialiased;cursor:text;position:relative} .rendered-markdown h1 tt, .rendered-markdown h1 code, .rendered-markdown h2 tt, .rendered-markdown h2 code, .rendered-markdown h3 tt, .rendered-markdown h3 code, .rendered-markdown h4 tt, .rendered-markdown h4 code, .rendered-markdown h5 tt, .rendered-markdown h5 code, .rendered-markdown h6 tt, .rendered-markdown h6 code{font-size:inherit} .rendered-markdown h1{font-size:28px;color:#000} .rendered-markdown h2{font-size:22px;border-bottom:1px solid #ccc;color:#000} .rendered-markdown h3{font-size:18px} .rendered-markdown h4{font-size:16px} .rendered-markdown h5{font-size:14px} .rendered-markdown h6{color:#777;font-size:14px} .rendered-markdown p, .rendered-markdown blockquote, .rendered-markdown ul, .rendered-markdown ol, .rendered-markdown dl, .rendered-markdown table, .rendered-markdown pre{margin:15px 0} .rendered-markdown hr{border:0 none;color:#ccc;height:4px;padding:0} .rendered-markdown>h2:first-child, .rendered-markdown>h1:first-child, .rendered-markdown>h1:first-child+h2, .rendered-markdown>h3:first-child, .rendered-markdown>h4:first-child, .rendered-markdown>h5:first-child, .rendered-markdown>h6:first-child{margin-top:0;padding-top:0} .rendered-markdown a:first-child h1, .rendered-markdown a:first-child h2, .rendered-markdown a:first-child h3, .rendered-markdown a:first-child h4, .rendered-markdown a:first-child h5, .rendered-markdown a:first-child h6{margin-top:0;padding-top:0} .rendered-markdown h1+p, .rendered-markdown h2+p, .rendered-markdown h3+p, .rendered-markdown h4+p, .rendered-markdown h5+p, .rendered-markdown h6+p{margin-top:0} .rendered-markdown ul, .rendered-markdown ol{padding-left:30px} .rendered-markdown ul li>:first-child, .rendered-markdown ul li ul:first-of-type, .rendered-markdown ol li>:first-child, .rendered-markdown ol li ul:first-of-type{margin-top:0} .rendered-markdown ul ul, .rendered-markdown ul ol, .rendered-markdown ol ol, .rendered-markdown ol ul{margin-bottom:0} .rendered-markdown dl{padding:0} .rendered-markdown dl dt{font-size:14px;font-weight:bold;font-style:italic;padding:0;margin:15px 0 5px} .rendered-markdown dl dt:first-child{padding:0} .rendered-markdown dl dt>:first-child{margin-top:0} .rendered-markdown dl dt>:last-child{margin-bottom:0} .rendered-markdown dl dd{margin:0 0 15px;padding:0 15px} .rendered-markdown dl dd>:first-child{margin-top:0} .rendered-markdown dl dd>:last-child{margin-bottom:0} .rendered-markdown blockquote{border-left:4px solid #DDD;padding:0 15px;color:#777} .rendered-markdown blockquote>:first-child{margin-top:0} .rendered-markdown blockquote>:last-child{margin-bottom:0} .rendered-markdown table th{font-weight:bold} .rendered-markdown table th, .rendered-markdown table td{border:1px solid #ccc;padding:6px 13px} .rendered-markdown table tr{border-top:1px solid #ccc;background-color:#fff} .rendered-markdown table tr:nth-child(2n){background-color:#f8f8f8} .rendered-markdown img{max-width:100%;-moz-box-sizing:border-box;box-sizing:border-box} .rendered-markdown code, .rendered-markdown tt{margin:0 2px;padding:0 5px;border:1px solid #eaeaea;background-color:#f8f8f8;border-radius:3px} .rendered-markdown code{white-space:nowrap} .rendered-markdown pre>code{margin:0;padding:0;white-space:pre;border:0;background:transparent} .rendered-markdown .highlight pre, .rendered-markdown pre{background-color:#f8f8f8;border:1px solid #ccc;font-size:13px;line-height:19px;overflow:auto;padding:6px 10px;border-radius:3px} .rendered-markdown pre code, .rendered-markdown pre tt{margin:0;padding:0;background-color:transparent;border:0}</style>
<div class="rendered-markdown"><h1>Quash Shell Implementation</h1>
<p>By Jamiliah Eubanks and Rabia Mamo</p>
<h2>Design Choices and Implementation Details</h2>
<p>Our implementation of the Quash shell focuses on modularity, readability, and efficient handling of various shell functionalities. Here's an overview of our design choices and implementation details:</p>
<h3>Command Structure</h3>
<p>We defined a <code>Command</code> struct to encapsulate command information:</p>
<pre><code class="c">typedef struct {
    char *args[MAX_ARGS];
    int argc;
    bool background;
} Command;
</code></pre>
<p>This structure allows for easy manipulation and passing of command data throughout the program.</p>
<h3>Main Loop</h3>
<p>The main loop of our shell follows this structure:</p>
<ol>
<li>Display the prompt with the current working directory</li>
<li>Read user input</li>
<li>Parse the input into a <code>Command</code> struct</li>
<li>Execute built-in commands or fork external processes</li>
</ol>
<p>This design provides a clear flow and separation of concerns.</p>
<h3>Built-in Commands</h3>
<p>We implemented built-in commands (cd, pwd, echo, exit, env, setenv) as separate functions, improving code organization and maintainability. The <code>execute_builtin</code> function uses a series of if-else statements to route commands to their respective functions.</p>
<h3>Process Execution</h3>
<p>For external commands, we use <code>fork()</code> and <code>execvp()</code>. The parent process waits for foreground processes to complete, while background processes run independently. We implemented a 10-second timer for foreground processes using <code>alarm()</code> and signal handling.</p>
<h3>Signal Handling</h3>
<p>We use signal handlers for SIGINT (Ctrl+C) and SIGALRM (timer expiration). This allows us to gracefully handle user interrupts and implement the process timeout feature.</p>
<h3>I/O Redirection and Piping</h3>
<p>We implemented basic I/O redirection for input (<code>&lt;</code>) and output (<code>&gt;</code>), as well as simple piping (<code>|</code>). The <code>handleIO</code> function manages redirection, while <code>executePipedCommands</code> handles piped commands.</p>
<h3>Environment Variables</h3>
<p>We support environment variable expansion in commands and implement the <code>setenv</code> built-in command for setting new environment variables.</p>
<h2>Code Documentation</h2>
<h3>Main Functions</h3>
<ul>
<li><code>main()</code>: The entry point of the shell, containing the main loop.</li>
<li><code>parse_input()</code>: Tokenizes user input and populates the <code>Command</code> struct.</li>
<li><code>execute_builtin()</code>: Handles execution of built-in commands.</li>
<li><code>execute_external()</code>: Forks and executes external commands.</li>
<li><code>executePipedCommands()</code>: Manages execution of piped commands.</li>
</ul>
<h3>Helper Functions</h3>
<ul>
<li><code>display_prompt()</code>: Prints the shell prompt with the current working directory.</li>
<li><code>handle_sigint()</code>: Signal handler for SIGINT.</li>
<li><code>handle_sigalrm()</code>: Signal handler for SIGALRM (process timeout).</li>
<li><code>handleIO()</code>: Manages I/O redirection.</li>
</ul>
<h3>Built-in Command Functions</h3>
<ul>
<li><code>changeDir()</code>: Implements the cd command.</li>
<li><code>getWorkingDir()</code>: Implements the pwd command.</li>
<li><code>getEnv()</code>: Implements the env command.</li>
<li><code>echoFunc()</code>: Implements the echo command.</li>
<li><code>setEnv()</code>: Implements the setenv command.</li>
<li><code>exitShell()</code>: Implements the exit command.</li>
</ul>
<h2>Challenges and Solutions</h2>
<ol>
<li><p><strong>Signal Handling</strong>: Implementing proper signal handling for SIGINT was challenging. We solved this by using a global variable to track the foreground process PID and only forwarding the signal to that process.</p>
</li>
<li><p><strong>Process Timeout</strong>: Implementing the 10-second timeout for foreground processes required careful use of <code>alarm()</code> and signal handling. We used SIGALRM to trigger the timeout and kill the process if necessary.</p>
</li>
<li><p><strong>I/O Redirection and Piping</strong>: Handling file descriptors correctly for I/O redirection and piping was complex. We solved this by carefully managing file descriptors and using <code>dup2()</code> to redirect input/output.</p>
</li>
<li><p><strong>Environment Variable Expansion</strong>: Implementing environment variable expansion in commands required modifying our parsing logic. We solved this by checking for '$' prefixes and using <code>getenv()</code> to expand variables.</p>
</li>
</ol>
<h2>Conclusion</h2>
<p>Our Quash shell implementation successfully meets the project requirements, providing a functional and extensible command-line interface. The modular design allows for easy addition of new features and improvements in the future. While there's always room for optimization, particularly in areas like error handling and more advanced piping scenarios, the current implementation provides a solid foundation for a basic shell program.</p>
<h1>OperatingSystems</h1>
</div>