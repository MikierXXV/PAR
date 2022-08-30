void mandelbrot(int height, int width, double real_min, double imag_min,
                double scale_real, double scale_imag, int maxiter, int **output) {

    // Calculate points and generate appropriate output
    #pragma omp parallel
    #pragma omp single
    for (int row = 0; row < height; ++row) {
    	//3.2 #pragma omp taskloop firstprivate(row) grainsize(1)
        for (int col = 0; col < width; ++col) {
        	//3.2 #pragma omp taskloop firstprivate(row, col) grainsize(1)
			
            #pragma omp task firstprivate(row, col)
            {
				complex z, c;
				z.real = z.imag = 0;

				/* Scale display coordinates to actual region  */
				c.real = real_min + ((double) col * scale_real);
				c.imag = imag_min + ((double) (height-1-row) * scale_imag);
											/* height-1-row so y axis displays
											 * with larger values at top
											 */

				// Calculate z0, z1, .... until divergence or maximum iterations
				int k = 0;
				double lengthsq, temp;
				do  {
					temp = z.real*z.real - z.imag*z.imag + c.real;
					z.imag = 2*z.real*z.imag + c.imag;
					z.real = temp;
					lengthsq = z.real*z.real + z.imag*z.imag;
					++k;
				} while (lengthsq < (N*N) && k < maxiter);

			output[row][col]=k;

				if (output2histogram) histogram[k-1]++;

				if (output2display) {
					/* Scale color and display point  */
					long color = (long) ((k-1) * scale_color) + min_color;
					if (setup_return == EXIT_SUCCESS) {
						// se genera datarace i ponen critical
						//pragma omp critical
						XSetForeground (display, gc, color);
						XDrawPoint (display, win, gc, col, row);
					}
				}
            }
        }
    }
}
      