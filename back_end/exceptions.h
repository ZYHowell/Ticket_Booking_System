#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

class exception {

};

class unallocated_space : public exception {
};

class non_existent_file : public exception {
};

class invalid_format : public exception {
};

class invalid_offset : public exception {
};

class invalid_iterator : public exception {
};

class not_found : public exception {
};

class index_out_of_bound : public exception {
	/* __________________________ */
};

class runtime_error : public exception {
	/* __________________________ */
};

class container_is_empty : public exception {
	/* __________________________ */
};

#endif
