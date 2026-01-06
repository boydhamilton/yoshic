# yoshi grammar

$$
\begin{align}

    \text{[program]} &\to \text{[stmt]}^+ \\

    \text{[stmt]} &\to 
    \begin{cases}
        exit \text{ [expr]}; \\
        let \text{ [ident]} = \text{[expr]}; \\
    \end{cases}
    \\

    \text{[expr]} &\to 
    \begin{cases}
        \text{[int\_lit]} \\
        \text{[ident]}
    \end{cases}
\end{align}
$$