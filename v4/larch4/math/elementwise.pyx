

cimport cython
from cython.parallel cimport prange

@cython.boundscheck(False)
cdef double _sum_of_elementwise_product(
		double[:,:] a,
		double[:,:] b,
):
	cdef Py_ssize_t a0 = a.shape[0]
	cdef Py_ssize_t a1 = a.shape[1]
	cdef Py_ssize_t i,j
	cdef double result =0
	#for i in prange(a0, nogil=True, schedule='static', chunksize=1000):
	for i in range(a0):
		for j in range(a1):
			if a[i,j]!=0:
				if b[i,j]!=0:
					result += a[i,j]*b[i,j]

	return result


def sum_of_elementwise_product(
		double[:,:] a,
		double[:,:] b,
):
	return _sum_of_elementwise_product(a,b)




@cython.boundscheck(False)
cdef void _sum_of_elementwise_product_save_dim_1(
		double[:,:] a,
		double[:,:] b,
		double[:] result
):
	cdef Py_ssize_t a0 = a.shape[0]
	cdef Py_ssize_t a1 = a.shape[1]
	cdef Py_ssize_t i,j
	#for i in prange(a0, nogil=True, schedule='static', chunksize=1000):
	for i in range(a0):
		result[i] = 0
		for j in range(a1):
			if a[i,j]!=0:
				if b[i,j]!=0:
					result[i] += a[i,j]*b[i,j]


def sum_of_elementwise_product_save_dim_1(
		double[:,:] a,
		double[:,:] b,
		double[:] result
):
	_sum_of_elementwise_product_save_dim_1(a,b,result)

