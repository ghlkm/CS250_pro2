all:
	mkdir -p obj
	nvcc -o obj/stencil main.cpp stencil.cu -O3 -g
