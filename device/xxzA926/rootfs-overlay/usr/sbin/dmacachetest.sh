mount -t debugfs none /mnt
echo 1 > /mnt/cpu_dma_test/run
echo 1 > /mnt/cycle_raw_dma_test
echo 1 > /mnt/l2c_test/run



