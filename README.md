# FileGen
## unix-linux command to create file header information
for command usage type :
```
$filegen --help
```

![filegen --help](./res/help.png)

```
$./filegen -a "PRAJWAL T R" -c "test file with py ext" temp.py
```

![filegen demo](./res/filegendemo.gif)

TODO :
- [x] add sys error functions and check return values for operation completion status
- [x] fork -> exec to default editor
- [x] fix new line issue while printing date to file
- [x] getting uname of node for author value
- [ ] add support for different files
