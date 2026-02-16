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
        if (\text{ [term] } )\\
        while (\text{ [term] } )\\
    \end{cases}
    \\

    \text{[expr]} &\to 
    \begin{cases}
        \text{[term]}\\
        \text{[bin\_expr]}
    \end{cases}
    \\

    \text{[bin\_expr]} &\to \begin{cases}
        \text{[expr]} * \text{[expr]} & \text{precedence} = 1\\
        \text{[expr] } / \text{ [expr]} & \text{precedence} = 1\\
        \text{[expr]} + \text{[expr]} & \text{precedence} = 0\\
        \text{[expr]} - \text{[expr]} & \text{precedence} = 0\\
    \end{cases}
    \\

    \text{[term]} &\to \begin{cases}
        \text{int\_lit}\\
        \text{ident} \\
        \text{([expr])}
    \end{cases}
    \\

\end{align}
$$