{
    void readMat(int N, int M)[int arr[N][M]]{
        for(int i : 0,N){
            for(int j : 0,M){
                STDIN >> arr[i][j];
            }
        }
    }

    STDIN >> int L;
    STDIN >> int M;
    STDIN >> int N;
    int mat1[L][M], mat2[M][N];

    readMat(L, M)[mat1];
    readMat(M, N)[mat2];

    void multiplyVM(int N, int M, int arr[N], int mat[N][M])[int ret[M]]{
        for(int i : 0,M){
            ret[i] = 0;
            for(int j : 0,N){
                ret[i] = ret[i] + arr[j]*mat[i][j];
            }
        }
    }

    int ret[L][N];
    for(int i : 0,L){
        multiplyVM(M, N, mat1[i], mat2)[ret[i]];
    }

    STDOUT << ret;
    STDOUT << '\n';
}