cpplint --counting=detailed $( find . -name *.hpp -or -name *.cpp -or -name *.c -or -name *.h | grep -vE "qpoases" | grep -vE "qpOASES" | grep -vE "acado_" | grep -vE "_export" | grep -vE "export_" )

