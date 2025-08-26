#ifdef UNIT_TESTING
#define TEST_FRIEND(classname) friend class classname
#else
#define TEST_FRIEND(classname)
#endif // UNIT_TESTING
