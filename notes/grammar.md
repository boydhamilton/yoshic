# yoshi grammar

$$
\begin{align}

    \text{[program]} &\to \text{[stmt]}^+ \\

    \text{[stmt]} &\to 
    \begin{cases}
        exit \text{ [expr]}; \\
        let \text{ ident} = \text{[expr]}; \\
        \text{ident} = \text{[expr]}; \\
        \{ \text{[stmt]}^+ \} \\
        if(\text{[expr]}) \text{[scope]} \\
        while(\text{[expr]}) \text{[scope]} \\
        \text{ident}(\text{[expr\_0]} , ... , \text{[expr\_n]}); \\
        funct \text{ ident}( \text{[expr\_0]} , ... , \text{[expr\_n]})
    \end{cases}
    \\

    \text{[scope]} &\to
        \{ \text{[stmt]}^+\}
    \\

    \text{[expr]} &\to 
    \begin{cases}
        \text{[term]}\\
        \text{[bin\_expr]} \\
    \end{cases}
    \\

    \text{[bin\_expr]} &\to \begin{cases}
        \text{[expr]} * \text{[expr]} & \text{precedence} = 1\\
        \text{[expr] } / \text{ [expr]} & \text{precedence} = 1\\
        \text{[expr]} + \text{[expr]} & \text{precedence} = 0\\
        \text{[expr]} - \text{[expr]} & \text{precedence} = 0\\
        \text{[conditional]} & \text{precedence} = -1
    \end{cases}
    \\

    \text{[unary\_expr]} &\to \begin{cases}
        !\text{[term]}\\
        -\text{[term]}
    \end{cases}
    \\

    \text{[term]} &\to \begin{cases}
        \text{int\_lit}\\
        \text{ident} \\
        \text{([expr])} \\
        \text{[unary\_expr]}
    \end{cases}
    \\

    \text{[conditional]} &\to \begin{cases}
        \text{[expr] } == \text{ [expr]} \\
        \text{[expr] } != \text{ [expr]} \\
        \text{[expr] } < \text{ [expr]} \\
        \text{[expr] } > \text{ [expr]} \\

    \end{cases}
    \\


\end{align}
$$