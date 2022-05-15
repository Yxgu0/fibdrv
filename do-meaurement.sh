#!/bin/bash
# This script works on 4 core computer to minimize the unstable factors on CPU 1
# while analyzing the performance of fibdrv.
CPUID=1
ORIG_ASLR=`cat /proc/sys/kernel/randomize_va_space`
ORIG_GOV=`cat /sys/devices/system/cpu/cpu$CPUID/cpufreq/scaling_governor`
ORIG_TURBO=`cat /sys/devices/system/cpu/intel_pstate/no_turbo`

sudo bash -c "echo 0 > /proc/sys/kernel/randomize_va_space"
sudo bash -c "echo performance > /sys/devices/system/cpu/cpu$CPUID/cpufreq/scaling_governor"
sudo bash -c "echo 1 > /sys/devices/system/cpu/intel_pstate/no_turbo"

# find all currently active IRQ number and change their smp_affinity
for file in `find /proc/irq -name "smp_affinity"` 
do
    orig=0x`cat ${file}`
    new="$(( $orig & 0xe ))"  # remove CPU 1 IRQ affinity
    new=`printf '%.1x' ${new}` # change decimal to hexdecimal
    sudo bash -c "echo ${new} > ${file}"
done
# remove CPU 1 from default IRQ affinity
sudo bash -c "echo e > /proc/irq/default_smp_affinity"

# measure the performance of fibdrv
make all
make unload
make load
rm -f plot_statistic
sudo taskset -c $CPUID ./client_statistic
sudo taskset -c $CPUID ./client_single
gnuplot scripts/plot_statistic.gp
gnuplot scripts/plot_single.gp
make unload

# restore the original system settings
sudo bash -c "echo $ORIG_ASLR >  /proc/sys/kernel/randomize_va_space"
sudo bash -c "echo $ORIG_GOV > /sys/devices/system/cpu/cpu$CPUID/cpufreq/scaling_governor"
sudo bash -c "echo $ORIG_TURBO > /sys/devices/system/cpu/intel_pstate/no_turbo"
sudo bash -c "echo f > /proc/irq/default_smp_affinity"