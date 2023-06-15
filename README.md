## Mnum - A Mutable number Class

Mnum is a mutable number data type that behaves like a number and provides additional mutable properties, including the ability to perform floating-point arithmetic with high accuracy. With Mnum, you can modify individual digits of a number using indexing and assign new values to them and more. Here's how to use it:

### Creating a Mnum object

To create a mnum object, simply call the mnum class with a number argument:

```python
from mnum import mnum

# create a mnum object with the value 56
x = mnum(56)
```

### Iterating over Digits

To iterate over the digits of a mnum object, you can simply use a for loop:

```python
for v in x:
    print(v)
```

This will output:

```
5
6
```

### Accessing and Replacing Digits

You can access individual digits of a mnum object using indexing, just like a string or a list:

```python
# get the second digit (0-based index)
d = x[1]

# replace the second digit with a new value
x[1] = 2
```

### Joining and Popping Digits

You can join two mnum objects together using the `join` method:

```python
# create two mnum objects
x = mnum(56)
y = mnum(78)

# join them together
x.join(y)

# now x has the value 5678
```

To remove and return the digit at a specific index, you can use the `pop` method:

```python
# remove and return the second digit (0-based index) from x
d = x.pop(1)

# now x has the value 578 and d has the value 6
```

### Removing Digits

To remove the first occurrence of a specific digit from a mnum object, you can use the `remove` method:

```python
# remove the first occurrence of the digit 5 from x
x.remove(5)

# now x has the value 68
```

### Arithmetic Operations

The Mnum supports arithmetic operations like addition, subtraction, multiplication, and division with high accuracy.

Here's an example:

```python
# create an mnum object with the value 0.1
x = mnum(0.1)

# add 0.2 to x
y = x + 0.2

# y now has the value 0.3
```

### Other Methods

mnum object also provides several other methods, including:

- `find(value)` - Returns the index of the first occurrence of value in the mnum object, or -1 if it is not found.

- `rfind(value)` - Returns the index of the last occurrence of value in the mnum object, or -1 if it is not found.

- `index(value)` - Returns the index of the first occurrence of value in the mnum object, or raises a ValueError exception if it is not found.

- `count(value)` - Returns the number of occurrences of value in the mnum object.

- `add(value)` - Adds the number value to the mnum object.

- `join(value)` - Joins the mnum object with value by concatenating them.

- `pop(index)` - Removes and returns the number at the specified index in the mnum object.

- `insert(index, value)` - Inserts the number value at the specified index in the mnum object.

- `remove(value)` - Removes the first occurrence of value in the mnum object, or raises a ValueError exception if it is not found.

- `clear()` - Removes all integers from the mnum object.

- `sort()` - Sorts the integers in the mnum object in ascending order.

- `reverse()` - Reverses the order of the integers in the mnum object.

- `__len__()` - Returns the length of the mnum object.

- `__delitem__`(index) - Removes the number at the specified index in the mnum object.

- `__getitem__`(index) - Returns the number at the specified index in the mnum object.

- `__setitem__`(index, value) - Replaces the number at the specified index in the mnum object with value.

Note that all of these methods behave in the same way as their counterparts in the built-in list class, except that they operate on Mnum objects instead of lists. Additionally, the Mnum class provides the unique mutable property of allowing its values to be indexed, replaced, and manipulated as separate digits.
For more information, please refer to the source code.

## Installing mnum
Here are the installation instructions for Mnum:

1. Ensure that you have a C++ compiler installed on your system. 

**For Windows users:**

If you have Visual Studio 2019 or a later version installed:

- Ensure that the Windows 10 SDK (10.0.18362 or later) is installed. If not, install it.

- Use Visual Studio's installer to install the "C++ for MFC for ..." package.

- Alternatively, you can install the Visual Studio Build Tools from the following link: 
https://visualstudio.microsoft.com/visual-cpp-build-tools/

3. Navigate to the root directory of the `mnum` source code in a command prompt or terminal.

4. Install requirements using command:
    ```bash
    pip install -r requirements.txt
    ```

5. Run the following command to install `mnum`:
    ```bash
    python setup.py install
    ```
    Alternatively use
    ```bash
    pip install .
    ```

This will compile the C++ code and install the Python package. You can then import mnum in your Python code and use it as desired.