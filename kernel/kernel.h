#ifndef __KERNEL_H__
#define __KERNEL_H__

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This system will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This system needs to be compiled with a ix86-elf compiler"
#endif

#endif