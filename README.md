# reinforth

A small implementation of Forth for fun, inspired by [forth.py](https://www.openbookproject.net/py4fun/forth/forth.html) in [Python for Fun](https://www.openbookproject.net/py4fun/)

Build debug version:

```
make
```

Build release version:

```
RELEASE=1 make
```

Run tests:

```
echo '"hello, world" print cr' | ./reinforth
make test
```

Format code:

```
make fmt
```

