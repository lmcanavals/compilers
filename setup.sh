#!/usr/bin/zsh

export MYDEPENDENCIES=$HOME/Apps

export CLASSPATH=.:$MYDEPENDENCIES/antlr-4.13.1-complete.jar:$CLASSPATH
alias antlr4="java org.antlr.v4.Tool"
alias grun='java org.antlr.v4.gui.TestRig'

export ANTLR4HOME=$MYDEPENDENCIES/antlr4-cpp/run
export ANTLR4I=$ANTLR4HOME/usr/local/include/antlr4-runtime
export ANTLR4L=$ANTLR4HOME/usr/local/lib/

