#$ -ckpt restart
module load cuda/10.1
hostname
./obj/stencil 3 init.dat n
