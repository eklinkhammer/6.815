#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <iostream>
#include <vector>
#include <math.h>
#include <stdexcept>

// calculate the cofactor of element (row,col)
static int GetMinor(double **src, double **dest, int row, int col, int order)
{
    // indicate which col and row is being copied to dest
    int colCount=0,rowCount=0;

    for(int i = 0; i < order; i++ )
    {
        if( i != row )
        {
            colCount = 0;
            for(int j = 0; j < order; j++ )
            {
                // when j is not the element
                if( j != col )
                {
                    dest[rowCount][colCount] = src[i][j];
                    colCount++;
                }
            }
            rowCount++;
        }
    }

    return 1;
}

// Calculate the determinant recursively.
static double CalcDeterminant( double **mat, int order)
{
    // order must be >= 0
    // stop the recursion when matrix is a single element
    if( order == 1 )
        return mat[0][0];

    // the determinant value
    double det = 0;

    // allocate the cofactor matrix
    double **minor;
    minor = new double*[order-1];
    for(int i=0;i<order-1;i++)
        minor[i] = new double[order-1];

    for(int i = 0; i < order; i++ )
    {
        // get minor of element (0,i)
        GetMinor( mat, minor, 0, i , order);
        // the recusion is here!

        det += (i%2==1?-1.0:1.0) * mat[0][i] * CalcDeterminant(minor,order-1);
        //det += pow( -1.0, i ) * mat[0][i] * CalcDeterminant( minor,order-1 );
    }

    // release memory
    for(int i=0;i<order-1;i++)
        delete [] minor[i];
    delete [] minor;

    return det;
}

// matrix inversioon: the result is put in Y
static void MatrixInversion(double **A, int order, double **Y)
{
    // get the determinant of a
    double det = 1.0/CalcDeterminant(A,order);

    // memory allocation
    double *temp = new double[(order-1)*(order-1)];
    double **minor = new double*[order-1];
    for(int i=0;i<order-1;i++)
        minor[i] = temp+(i*(order-1));

    for(int j=0;j<order;j++)
    {
        for(int i=0;i<order;i++)
        {
            // get the co-factor (matrix) of A(j,i)
            GetMinor(A,minor,j,i,order);
            Y[i][j] = det*CalcDeterminant(minor,order-1);
            if( (i+j)%2 == 1)
                Y[i][j] = -Y[i][j];
        }
    }

    // release memory
    //delete [] minor[0];
    delete [] temp;
    delete [] minor;
}


/**
 * Class to create square matrices of any size
 *
 * Contructor:
 * Initialize with a raw double array, must of size order x order else
 * seg fault will ensue
 *
 * Accessors:
 *  M(x,y) or M.at(x,y) return mutable reference of the matrix element
 *  at (x,y) coordinate which is also the element at row y and column x
 *
 */
class Matrix
{
private:
    int row;
    int col;
    std::vector< std::vector<float> > mat;

public:
    Matrix(int c, int r) {
        row = r;
        col = c;
        mat.resize(rows());
        for (int y=0; y<rows(); y++) {
            mat[y].resize(columns(), 0.0f);
        }
    }

    Matrix(int c, int r, float* data) {
        row = r;
        col = c;
        mat.resize(rows());
        for (int y=0; y<rows(); y++) {
            mat[y].resize(columns(), 0.0f);
        }
        for (int y=0; y<rows(); y++) {
            for (int x=0; x<columns(); x++) {
                at(x,y) = data[ columns()*y+x ];
            }
        }
    }

    ~Matrix(void) {}

    int rows(void) {
        return row;
    }

    int columns(void) {
        return col;
    }

    float& at(int x, int y) {
        return mat[y][x];
    }

    float& operator()(int x, int y) {
        return at(x,y);
    }

    void print(void) {
        for (int y=0; y<rows(); y++) {
            for (int x=0; x<columns(); x++) {
                std::cout << at(x,y) << "\t";
            }
            std::cout << "\n";
        }
        std::cout << std::endl;
    }

    Matrix inverse(void) {
        if (rows() != columns()) {
            throw std::runtime_error("Matrix inverse cannot be computed for non-square matrices, use pseudo inverse Matrix::pinverse()");
        }

        double **in_array  = new double*[rows()];
        double **out_array = new double*[rows()];

        // allocate raw double arrays
        for (int y=0; y<rows(); y++) {
            in_array[y]  = new double[columns()];
            out_array[y] = new double[columns()];
        }

        // convert input matrix to double array
        for (int y=0; y<rows(); y++) {
            for (int x=0; x<columns(); x++) {
                in_array[y][x] = mat[y][x];
            }
        }

        // compute inverse
        MatrixInversion(in_array, rows(), out_array);

        // populate output matrix
        Matrix out(columns(), rows());
        for (int y=0; y<out.rows(); y++) {
            for (int x=0; x<out.columns(); x++) {
                out.at(x,y) = out_array[y][x];
            }
        }

        // deallocate
        for (int y=0; y<rows(); y++) {
            delete [] in_array[y];
            delete [] out_array[y];
        }
        delete [] in_array;
        delete [] out_array;

        return out;
    }

    Matrix multiply(Matrix b) {
        int ra = rows();
        int ca = columns();

        int rb = b.rows();
        int cb = b.columns();

        if (ca != rb) {
            throw std::runtime_error("Matrix dimensions do not agree for multiplication");
        }

        Matrix out(cb, ra);
        for (int i=0; i<ra; i++) {
            for (int j=0; j<cb; j++) {
                for (int k=0; k<ca; k++) {
                    out.at(j,i) += at(k,i) * b.at(j,k);
                }
            }
        }

        return out;
    }

    double determinant(void) {
        if (rows() != columns()) {
            throw std::runtime_error("Matrix determinant cannot be computed for non-square matrices");
        }

        double **in_array  = new double*[rows()];

        // allocate raw double arrays
        for (int y=0; y<rows(); y++) {
            in_array[y]  = new double[columns()];
        }

        // convert input matrix to double array
        for (int y=0; y<rows(); y++) {
            for (int x=0; x<columns(); x++) {
                in_array[y][x] = mat[y][x];
            }
        }

        // deallocate
        for (int y=0; y<rows(); y++) {
            delete [] in_array[y];
        }
        delete [] in_array;

        return CalcDeterminant(in_array, rows());
    }

    Matrix transpose(void) {
        Matrix out(rows(), columns());
        for (int i=0; i<out.rows(); i++) {
            for (int j=0; j<out.columns(); j++) {
                out.at(j,i) = at(i,j);
            }
        }
        return out;
    }

    Matrix pinverse(void) {
        Matrix A  = *this;
        Matrix At = A.transpose();
        return ((At.multiply(A)).inverse()).multiply(At);
    }
};

#endif // _MATRIX_H_
