export CPPFLAGS="-isystem /usr/local/include -isystem /usr/local/opt/readline/include" LDFLAGS="-L/usr/local/lib -L/usr/local/opt/readline/lib"
./configure --prefix=$HOME/local/groovx --with-tcltk=$HOME/local/tcl-8.6.6
