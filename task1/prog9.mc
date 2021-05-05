{
    void readMat(int N, int M)[int arr[N][M]]{
        for(int i : 0,N){
            for(int j : 0,M){
                STDIN >> arr[i][j];
            }
        }
    }

    int N, M;
    STDIN >> N;
    STDIN >> M;
    int mat1[N][M], mat2[N][M];

    readMat(N, M)[mat1];
    readMat(N, M)[mat2];

    for(int i : 0,N){
        for(int j :0,M){
            STDOUT << mat1[i][j] + mat2[i][j];
            STDOUT << ' ';
        }
        STDOUT << '\n';
    }
}