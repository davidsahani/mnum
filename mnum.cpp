#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "include/mnum.h"

const mnum cast(const pybind11::handle &handle)
{
     if (pybind11::isinstance<mnum>(handle))
     {
          // Handle is already a mint object
          return handle.cast<mnum>();
     }
     if (PyLong_Check(handle.ptr()))
     {
          // Convert Python int to C++ long long and then to mint
          const long long n = PyLong_AsLongLong(handle.ptr());
          if (n == -1 && PyErr_Occurred())
               throw pybind11::error_already_set();
          return mnum(n);
     }
     if (PyUnicode_Check(handle.ptr()))
     {
          // Convert Python str to C++ string and then to mint
          const std::string s = PyUnicode_AsUTF8(handle.ptr());
          return mnum(s);
     }
     if (pybind11::isinstance<pybind11::float_>(handle))
     {
          try
          {
               // get a string representation of the object
               std::string str = pybind11::str(handle.ptr());
               return mnum(str);
          }
          catch(const std::invalid_argument&)
          {
               throw pybind11::value_error("can't convert float with scientific notation");
          }
     }
     // Raise TypeError for unsupported types
     throw pybind11::type_error("unsupported type, expected int, float, str or mnum");
}

const size_t check_index(const mnum &self, const int_fast64_t &index)
{
     const size_t size = self.size();
     const size_t idx = (index < 0) ? index + size : index;
     if (idx >= size)
          throw pybind11::index_error("index out of range");
     return idx;
}

udigit to_digit(const pybind11::handle &handle, bool &sign)
{
     if (PyLong_Check(handle.ptr()))
     {
          // Convert Python int to C++ long long and then to digit
          const long long n = PyLong_AsLongLong(handle.ptr());
          if (n == -1 && PyErr_Occurred())
               throw pybind11::error_already_set();
          if (std::abs(n) > 9)
               throw pybind11::value_error("int must be a single digit");
          sign = (n < 0);
          return static_cast<udigit>(std::abs(n));
     }
     if (pybind11::isinstance<mnum>(handle))
     {
          const mnum m = handle.cast<mnum>();
          if (!(m.fraction.size() == 1 && m.fraction[0] == 0))
               throw pybind11::value_error("mnum must be non-fraction");
          if (m.integer.size() != 1)
               throw pybind11::value_error("mnum must be single digit");
          sign = m.sign;
          return static_cast<udigit>(m.integer[0]);
     }
     if (PyLong_Check(handle.attr("__int__")().ptr()))
     {
          // Convert Python object to int and then to digit
          const int x = handle.cast<int>();
          sign = (x < 0);
          if (std::abs(x) > 9)
               throw pybind11::value_error("int must be single digit");
          return static_cast<udigit>(std::abs(x));
     }
     throw pybind11::type_error("unsupported type, expected int or mnum");
}

namespace py = pybind11;

PYBIND11_MODULE(mnum, m)
{
     py::class_<mnum>(m, "mnum")
         .def(py::init([](const py::handle& handle)
               { return mnum(cast(handle)); }))
         .def("__bool__", [](const mnum &self) -> bool
              { return !(self.integer.size() == 1 && self.integer[0] == 0 &&
                       self.fraction.size() == 1 && self.fraction[0] == 0);})
         .def("__eq__", [](const mnum &self, const py::handle &other) -> bool
              { return self == cast(other); })
         .def("__ne__", [](const mnum &self, const py::handle &other) -> bool
              { return self != cast(other); })
         .def("__lt__", [](const mnum &self, const py::handle &other) -> bool
              { return self < cast(other); })
         .def("__le__", [](const mnum &self, const py::handle &other) -> bool
              { return self <= cast(other); })
         .def("__gt__", [](const mnum &self, const py::handle &other) -> bool
              { return self > cast(other); })
         .def("__ge__", [](const mnum &self, const py::handle &other) -> bool
              { return self >= cast(other); })
         .def("__contains__", [](const mnum &self, const py::handle &other) -> bool
              { return self.contains(cast(other)); })
         .def("__pos__", [](const mnum &self) -> mnum
              { return +self; })
         .def("__neg__", [](const mnum &self) -> mnum
              { return -self; })
         .def("__abs__", [](const mnum &self) -> mnum
              { return self.abs(); })
         .def("__int__", [](const mnum &self) -> int_fast64_t
          {
               int_fast64_t n = 0;
               const size_t size = self.integer.size();
               for (size_t i = 0; i < size; ++i)
                    n  = n * 10 + self.integer[i];
               return self.sign ? -n : n;
          })
         .def("__float__", [](const mnum &self) -> double
          {
               const size_t int_size = self.integer.size();
               const size_t frac_size = self.fraction.size();
               size_t n = 0;
               for (size_t i = 0; i < int_size; ++i)
                    n  = n * 10 + self.integer[i];
               size_t f = 0;
               for (size_t i = 0; i < frac_size; ++i)
                    f = f * 10 + self.fraction[i];
               const double d = f / std::pow(10, frac_size);
               return self.sign ? -(n + d) : (n + d);
          })
         .def("as_int", [](const mnum &self) -> mnum
              { return self.as_int(); })
         .def("as_float", [](const mnum &self) -> mnum
              { return self.as_float(); })
         .def("int_part", [](const mnum &self) -> mnum
              { return self.as_int(); })
         .def("frac_part", [](const mnum &self) -> mnum
              { return self.frac_part(); })
         .def("__repr__", [](const mnum &self) -> std::string
              { return self.str(); })
         .def("float_str", [](const mnum &self) -> std::string
              { return self.float_str(); })
         .def("__add__", [](const mnum &self, const py::handle &other) -> mnum
              { return self + cast(other); })
         .def("__radd__", [](const mnum &self, const py::handle &other) -> mnum
              { return cast(other) + self; })
         .def("__iadd__", [](mnum &self, const py::handle &other) -> mnum
          {
               self += cast(other);
               return self;
          })
         .def("__sub__", [](const mnum &self, const py::handle &other) -> mnum
              { return self - cast(other); })
         .def("__rsub__", [](const mnum &self, const py::handle &other) -> mnum
              { return cast(other) - self; })
         .def("__isub__", [](mnum &self, const py::handle &other) -> mnum
          {
               self -= cast(other);
               return self;
          })
         .def("__mul__", [](const mnum &self, const py::handle &other) -> mnum
              { return self * cast(other); })
         .def("__rmul__", [](const mnum &self, const py::handle &other) -> mnum
              { return cast(other) * self; })
         .def("__imul__", [](mnum &self, const py::handle &other) -> mnum
          {
               self *= cast(other);
               return self;
          })
         .def("div", [](const mnum &self, const py::handle &other) -> mnum
          {
               try
               {
                    return self.div(cast(other));
               } catch(const std::domain_error& e)
               {
                    PyErr_SetString(PyExc_ZeroDivisionError, e.what());
                    throw py::error_already_set();
               }
          })
         .def("idiv", [](mnum &self, const py::handle &other) -> mnum
          {
               try
               {
                    self.idiv(cast(other));
                    return self;
               } catch(const std::domain_error& e)
               {
                    PyErr_SetString(PyExc_ZeroDivisionError, e.what());
                    throw py::error_already_set();
               }
          })
         .def("__floordiv__", [](const mnum &self, const py::handle &other) -> mnum
          {
               try
               {
                    return self.floor_div(cast(other));
               } catch(const std::domain_error& e)
               {
                    PyErr_SetString(PyExc_ZeroDivisionError, e.what());
                    throw py::error_already_set();
               }
          })
         .def("__rfloordiv__", [](const mnum &self, const py::handle &other) -> mnum
          {
               try
               {
                    return cast(other).floor_div(self);
               } catch(const std::domain_error& e)
               {
                    PyErr_SetString(PyExc_ZeroDivisionError, e.what());
                    throw py::error_already_set();
               }
          })
         .def("__ifloordiv__", [](mnum &self, const py::handle &other) -> mnum
          {
               try
               {
                    self.ifloor_div(cast(other));
                    return self;
               } catch(const std::domain_error& e)
               {
                    PyErr_SetString(PyExc_ZeroDivisionError, e.what());
                    throw py::error_already_set();
               }
          })
         .def("__truediv__", [](const mnum &self, const py::handle &other) -> mnum
          {
               try
               {
                    return self / cast(other);
               }
               catch (const std::domain_error &e)
               {
                    PyErr_SetString(PyExc_ZeroDivisionError, e.what());
                    throw py::error_already_set();
               }
          })
         .def("__rtruediv__", [](const mnum &self, const py::handle &other) -> mnum
          {
               try
               {
                    return cast(other) / self;
               }
               catch (const std::domain_error &e)
               {
                    PyErr_SetString(PyExc_ZeroDivisionError, e.what());
                    throw py::error_already_set();
               }
          })
         .def("__itruediv__", [](mnum &self, const py::handle &other) -> mnum
          {
               try
               {
                    self /= cast(other);
                    return self;
               }
               catch (const std::domain_error &e)
               {
                    PyErr_SetString(PyExc_ZeroDivisionError, e.what());
                    throw py::error_already_set();
               }
          })
         .def("__mod__", [](const mnum &self, const py::handle &other) -> mnum
          { 
               try
               {
                    return self % cast(other);
               }
               catch (const std::domain_error &e)
               {
                    PyErr_SetString(PyExc_ZeroDivisionError, e.what());
                    throw py::error_already_set();
               }
          })
         .def("__rmod__", [](const mnum &self, const py::handle &other) -> mnum
          {
               try
               {
                    return cast(other) % self;
               }
               catch (const std::domain_error &e)
               {
                    PyErr_SetString(PyExc_ZeroDivisionError, e.what());
                    throw py::error_already_set();
               }
          })
         .def("__imod__", [](mnum &self, const py::handle &other) -> mnum
          {
               try
               {
                    self %= cast(other);
                    return self;
               }
               catch (const std::domain_error &e)
               {
                    PyErr_SetString(PyExc_ZeroDivisionError, e.what());
                    throw py::error_already_set();
               }
          })
         .def("__pow__", [](const mnum &self, const py::object &other) -> mnum
              { return self.pow(cast(other)); })
         .def("__rpow__", [](const mnum &self, const py::object &other) -> mnum
              { return cast(other).pow(self); })
         .def("__len__", [](const mnum &self) -> size_t
              { return self.size(); })
         .def("int_len", [](const mnum &self) -> size_t
              { return self.integer.size(); })
         .def("frac_len", [](const mnum &self) -> size_t
          {
               const size_t frac_size = self.fraction.size();
               const bool is_frac_zero = frac_size == 1 && self.fraction[0] == 0;
               return is_frac_zero ? 0 : frac_size; // fraction length
          })
         .def("find", [](const mnum &self, const py::handle &value) -> int_fast64_t
          {
               bool sign;
               const udigit v = to_digit(value, sign);
               if (sign != self.sign)
                    return -1;
               return self.find(v); // found value
          })
         .def("rfind", [](const mnum &self, const py::handle &value) -> int_fast64_t
          {
               bool sign;
               const udigit v = to_digit(value, sign);
               if (sign != self.sign)
                    return -1;
               return self.rfind(v); // found value
          })
         .def("index", [](const mnum &self, const py::handle &value) -> size_t
          {
               bool sign;
               const udigit v = to_digit(value, sign);
               if (sign != self.sign)
                    throw py::value_error("self and value signs do not match");
               return self.index(v); // found value
          })
         .def("count", [](const mnum &self, const py::handle &value) -> size_t
          {
               bool sign;
               const udigit v = to_digit(value, sign);
               if (sign != self.sign)
                    return 0;
               return self.count(v); // value count
          })
         .def("join", [](mnum &self, const py::handle &other) -> void
              { self.join(cast(other)); })
         .def("add", [](const mnum &self, const py::handle &other) -> mnum
              { return self.add(cast(other)); })
         .def("__getitem__", [](const mnum &self, const int_fast64_t &index) -> mnum
          {
               const size_t idx = check_index(self, index);
               const size_t int_size = self.integer.size();
               if (idx < int_size)
                    return mnum({self.integer[idx]}, {0}, self.sign);
               return mnum({self.fraction[idx - int_size]}, {0}, self.sign); // value
          })
         .def("geti", [](const mnum &self, const int_fast64_t &index) -> py::object
          {
               const size_t size = self.integer.size();
               const size_t idx = (index < 0) ? index + size : index;
               if (idx >= size)
                    return py::none();
               return py::cast(mnum({self.integer[idx]}, {0}, self.sign));
          })
         .def("getf", [](const mnum &self, const int_fast64_t &index) -> py::object
          {
               const size_t size = self.fraction.size();
               const size_t idx = (index < 0) ? index + size : index;
               if (idx >= size)
                    return py::none();
               return py::cast(mnum({self.fraction[idx]}, {0}, self.sign));
          })
         .def("__setitem__", [](mnum &self, const int_fast64_t &index, const py::handle &value) -> void
          {
               bool sign;
               const size_t idx = check_index(self, index);
               const udigit d_value = to_digit(value, sign);
               self.sign = self.sign ^ sign;
               if (idx < self.integer.size())
               {
                    self.integer[idx] = d_value;
                    lstrip_zeros(self.integer); // normalize integer
                    return;
               }
               self.fraction[idx - self.integer.size()] = d_value;
               rstrip_zeros(self.fraction); // normalize fraction
          })
         .def("insert", [](mnum &self, const int_fast64_t &index, const py::handle &value) -> void
          {
               const size_t size = self.size();
               const size_t idx = (index < 0) ? index + size : index;
               bool sign; // sign of value
               const udigit v = to_digit(value, sign);
               if (idx < size)
                    self.insert(idx, v);
               else if (index < 0)
                    self.insert(0, v);
               else
               { // insert at the end
                    if (self.fraction.size() == 1 &&
                         self.fraction[0] == 0)
                         self.integer.push_back(v);
                    else
                         self.fraction.push_back(v);
               }
               self.sign = self.sign ^ sign; // toggle sign
          })
         .def("__delitem__", [](mnum &self, const int_fast64_t &index) -> void
          {
               const size_t idx = check_index(self, index);
               self.erase(idx); // del the value
          })
         .def("pop", [](mnum &self, const int_fast64_t &index = -1) -> mnum
          {
               const size_t idx = check_index(self, index);
               return self.pop(idx); // pop the value
          })
         .def("remove", [](mnum &self, py::handle &value) -> void
          {
               bool sign;
               const udigit v = to_digit(value, sign);
               if (sign != self.sign)
                    return;
               self.remove(v); // remove value
          })
         .def("clear", [](mnum &self) -> void
              { self.clear(); })
         .def("sort", [](mnum &self) -> void
              { self.sort(); })
         .def("reverse", [](mnum &self) -> void
              { self.reverse(); });
}