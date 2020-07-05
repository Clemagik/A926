
echo ------ ddr perf test ........

while true
do
ddr_test_nosimd -c -w -p 4M -l 100 &
ddr_test_simd -c -w -p 4M -l 100
done





