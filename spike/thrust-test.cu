#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/complex.h>
#include <thrust/device_ptr.h>
#include <thrust/copy.h>

#include <iostream>
#include <vector>
#include <complex>

#include "../include/hpc_helpers.hpp"

__global__ void print_stuff(thrust::complex<float>* c, int N)
{
    int idx = blockDim.x*blockIdx.x + threadIdx.x;

    printf("id: %d, N: %d\n", idx, N);

    if(idx<N) {
        printf("id: %d real: %f imag: %f\n", idx, c[idx].real(), c[idx].imag());
        //printf("hello\n");
    }
}

int main(void)
{

    std::vector<std::complex<float> > H(1000);

    int N = 50;
    for(int i=0; i<H.size(); ++i) {
        H[i].real((float)i/N);
        H[i].imag(-i%N);
    }

    thrust::host_vector<thrust::complex<float> > h2(H.begin(), H.end());

    //~ for(int i=0; i<h2.size(); ++i)
        //~ h2[i]=H[i];

    //~ std::vector<int > H(1000);

    //~ int N = 50;
    //~ for(int i=0; i<H.size(); ++i) {
        //~ H[i] = i%N;
    //~ }

    //~ thrust::complex<float>* dev;

    //~ std::cout << "malloc" << std::endl;
	//~ cudaMalloc(&dev, h2.size()*sizeof(thrust::complex<float>));   		CUERR

    //~ std::cout << "copy" << std::endl;
    //~ thrust::copy(h2.begin(), h2.end(), thrust::device_ptr<thrust::complex<float> >(dev));                      CUERR

   //~ // std::cout << "cast" << std::endl;
   //~ // std::complex<float>* std_dev = (std::complex<float>*) dev;

    std::cout << "declare" << std::endl;
    thrust::device_vector<thrust::complex<float> > D(h2.begin(), h2.end()); CUERR //D(H.begin(), H.end()); CUERR

    std::cout << "cast" << std::endl;
    thrust::complex<float>* dev = thrust::raw_pointer_cast(D.data());

    std::complex<float>* std_dev = (std::complex<float>*) dev;

    std::cout << H.size() << std::endl;

    int threads = 256;
    int blocks = SDIV(H.size(), threads);
    print_stuff<<<threads, blocks >>>((thrust::complex<float>*)std_dev, H.size());           CUERR
    std::cout << "end" << std::endl;

    return 0;
}
