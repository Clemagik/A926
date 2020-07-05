#ifndef NATIVEHELPER_JNIHELP_H_
#define NATIVEHELPER_JNIHELP_H_

#if !defined(DISALLOW_COPY_AND_ASSIGN)
// DISALLOW_COPY_AND_ASSIGN disallows the copy and operator= functions. It goes in the private:
// declarations in a class.
#if __cplusplus >= 201103L
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;  \
  void operator=(const TypeName&) = delete
#else
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);  \
  void operator=(const TypeName&)
#endif  // __has_feature(cxx_deleted_functions)
#endif  // !defined(DISALLOW_COPY_AND_ASSIGN)

#endif  /* NATIVEHELPER_JNIHELP_H_ */
