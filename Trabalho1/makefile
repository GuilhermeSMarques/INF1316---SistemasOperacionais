all: KernelSim InterControllerSim Aplicacao CLI

KernelSim: KernelSim.c
				gcc -o KernelSim KernelSim.c

InterControllerSim: InterControllerSim.c
				gcc -o InterControllerSim InterControllerSim.c

Aplicacao: Aplicacao.c
				gcc -o aplicacao Aplicacao.c

CLI: cli.c
				gcc -o cli cli.c
clean:
				rm -f KernelSim InterControllerSim aplicacao