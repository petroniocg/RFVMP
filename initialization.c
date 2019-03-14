/* 
 * initialization.c: Random Fit Algorithm for Virtual Machine Placement - Initialization
 * Date: 14-09-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
 * 
 * This code is based in imavmp.c developed by Fabio Lopez Pires, as follows
 * initialization.c: Virtual Machine Placement Problem - Initialization Stage
 * Date: 17-11-2014
 * Author: Fabio Lopez Pires (flopezpires@gmail.com)
 * Corresponding Conference Paper: A Many-Objective Optimization Framework for Virtualized Datacenters
 * Available at: https://github.com/flopezpires
*/


/* include initialization stage header */
#include "initialization.h"
#include "commitment.h"
#include "common.h"

/* initialization: initializes a population randomly
 * parameter: number of physical machines
 * parameter: number of virtual machines
 * parameter: virtual machines requirements matrix
 * returns: population matrix
*/
int* initialization(int h_size, int v_size, int **V, int CRITICAL_SERVICES)
{
	// population: initial population matrix 
	int *population = (int *) malloc (v_size * sizeof (int));

	// iterator
	int iterator_virtual;

	// iterate on vms 
	for (iterator_virtual=0; iterator_virtual < v_size; iterator_virtual++)
	{
		// assign 0 to each virtual machine 
		population[iterator_virtual] = 0;
	}
	return population;
}

/* allocate: allocates the VMs into PMs randomly
 * parameter: population vector
 * parameter: utilization matrix
 * parameter: commitment vector
 * parameter: physical machines matrix
 * parameter: virtual machines requirements matrix
 * parameter: number of physical machines
 * parameter: number of virtual machines
 * returns: population matrix
*/
int* allocate(int *population, int **utilization, float *cm, int **H, int **V, int h_size, int v_size, int CRITICAL_SERVICES)
{
	/* iterators */
	int iterator_virtual = 0;
	int iterator_physical = 0;
	//int last_vm = 0;

	int count = 1;

	FILE *rfvmp_result;

	float available_cpus = 0.0, available_memory = 0.0;

	for (iterator_virtual = 0; iterator_virtual < v_size; iterator_virtual++)
	{
		/* Generates a int position representing a PM from 0 to h_size */
		iterator_physical = generate_solution_position(h_size);
		count = 1;

		/* Tries 2*h_size to find a no overloaded PM randomly */
		while (is_overloaded(H, utilization, cm, iterator_physical) && count <= 2*h_size)
		{
			iterator_physical = generate_solution_position(h_size);
			count++;
		}

		if ( count < 2*h_size)
		{
			if ( V[iterator_virtual][3] == CRITICAL_SERVICES )
			{
				available_cpus = (float) H[iterator_physical][0];
				available_memory = (float) H[iterator_physical][1];

				if ( (float) utilization[iterator_physical][0] + (float) V[iterator_virtual][0] <= available_cpus &&
					(float) utilization[iterator_physical][1] + (float) V[iterator_virtual][1] <= available_memory && 
					(float) utilization[iterator_physical][2] + (float) V[iterator_virtual][2] <= (float) H[iterator_physical][2])
				{
					/* add requirements from physical machine migration destination */
					utilization[iterator_physical][0] += V[iterator_virtual][0];
					utilization[iterator_physical][1] += V[iterator_virtual][1];
					utilization[iterator_physical][2] += V[iterator_virtual][2];

					/* refresh the population */
					population[iterator_virtual] = iterator_physical + 1;
					number_alocated_vms++;
					cm[iterator_physical] = CL_CS;
				}
			}
			else
			{
				if (cm[iterator_physical] > 1.0)
				{
					available_cpus = (float) H[iterator_physical][0] * cm[iterator_physical];
					available_memory = (float) H[iterator_physical][1] * cm[iterator_physical];
				}
				else
				{
					available_cpus = (float) H[iterator_physical][0];
					available_memory = (float) H[iterator_physical][1];
				}				
				if ( (float) utilization[iterator_physical][0] + (float) V[iterator_virtual][0] <= available_cpus &&
					(float) utilization[iterator_physical][1] + (float) V[iterator_virtual][1] <= available_memory && 
					(float) utilization[iterator_physical][2] + (float) V[iterator_virtual][2] <= (float) H[iterator_physical][2])
				{
					/* add requirements from physical machine */
					utilization[iterator_physical][0] += V[iterator_virtual][0];
					utilization[iterator_physical][1] += V[iterator_virtual][1];
					utilization[iterator_physical][2] += V[iterator_virtual][2];

					/* refresh the population */
					population[iterator_virtual] = iterator_physical + 1;
					number_alocated_vms++;
					if ( cm[iterator_physical] == CL_NC || cm[iterator_physical] == 0 )
						cm[iterator_physical] = CL_NC;
				}
			}
		}
	}

	/* Last try to allocate VMs that were not allocated. This time, using First Fit */
	if(number_alocated_vms < v_size)
	{
		for (iterator_virtual = 0; iterator_virtual < v_size; iterator_virtual++)
		{
			/* If the indication of PM is zero, the VM was not allocated */
			if (population[iterator_virtual] == 0)
			{
				for (iterator_physical = 0; iterator_physical < h_size ; iterator_physical++)
				{
					if ( V[iterator_virtual][3] == CRITICAL_SERVICES )
					{
						available_cpus = (float) H[iterator_physical][0];
						available_memory = (float) H[iterator_physical][1];

						if ( (float) utilization[iterator_physical][0] + (float) V[iterator_virtual][0] <= available_cpus &&
							(float) utilization[iterator_physical][1] + (float) V[iterator_virtual][1] <= available_memory && 
							(float) utilization[iterator_physical][2] + (float) V[iterator_virtual][2] <= (float) H[iterator_physical][2])
						{
							//* add requirements from physical machine migration destination
							utilization[iterator_physical][0] += V[iterator_virtual][0];
							utilization[iterator_physical][1] += V[iterator_virtual][1];
							utilization[iterator_physical][2] += V[iterator_virtual][2];

							//* refresh the population
							population[iterator_virtual] = iterator_physical + 1;
							number_alocated_vms++;
							cm[iterator_physical] = CL_CS;
							break;
						}
					}
					else
					{
						if (cm[iterator_physical] > 1.0)
						{
							available_cpus = (float) H[iterator_physical][0] * cm[iterator_physical];
							available_memory = (float) H[iterator_physical][1] * cm[iterator_physical];
						}
						else
						{
							available_cpus = (float) H[iterator_physical][0];
							available_memory = (float) H[iterator_physical][1];
						}
						
						if ( (float) utilization[iterator_physical][0] + (float) V[iterator_virtual][0] <= available_cpus &&
							(float) utilization[iterator_physical][1] + (float) V[iterator_virtual][1] <= available_memory && 
							(float) utilization[iterator_physical][2] + (float) V[iterator_virtual][2] <= (float) H[iterator_physical][2])
						{
							//* add requirements from physical machine
							utilization[iterator_physical][0] += V[iterator_virtual][0];
							utilization[iterator_physical][1] += V[iterator_virtual][1];
							utilization[iterator_physical][2] += V[iterator_virtual][2];

							//* refresh the population
							population[iterator_virtual] = iterator_physical + 1;
							number_alocated_vms++;
							if ( cm[iterator_physical] == CL_NC || cm[iterator_physical] == 0 )
								cm[iterator_physical] = CL_NC;
							
							break;
						}
					}
				}
			}
		}
		if (v_size-number_alocated_vms > 0)
		{
			rfvmp_result = fopen("results/rfvmp_result","a");
			printf("\n[WARNING]: It was not possible to allocate %d VMs.\n", v_size - number_alocated_vms);
			fprintf(rfvmp_result,"\n[WARNING]: It was not possible to allocate %d VMs.\n", v_size - number_alocated_vms);
			fclose(rfvmp_result);
		}
	}

	if (iterator_virtual < v_size)
	{
		rfvmp_result = fopen("results/rfvmp_result","a");
		printf("\n[WARNING]: There are less Physical Machines than needed! Only %d VMs allocated.\n", number_alocated_vms);
		fprintf(rfvmp_result,"\n[WARNING]: There are less Physical Machines than needed! Only %d VMs allocated.\n", number_alocated_vms);
		fclose(rfvmp_result);
	}
	return population;
}


/* is_overloaded: verificates if a physical machine is overloaded
 * parameter: physical machine resources matrix
 * parameter: utilization of the physical machines matrix
 * parameter: commitment vector 
 * parameter: identificator of the physical machine
 * returns: 1 if yes, 0 if no
*/
int is_overloaded(int **H, int **utilization, float *cm, int physical)
{
	//* If the use of the VM exceeds the capacity of the physical machine return 1, otherwise return 0
	if ( ( (float) utilization[physical][0] > (float) H[physical][0] * cm[physical] )
	||   ( (float) utilization[physical][1] > (float) H[physical][1] * cm[physical] )
	||   ( (float) utilization[physical][2] > (float) H[physical][2] ) )
	{
		return 1;
	}
	return 0;
}


/* generate_solution_position: generates a host number between 0 and parameter max_posible
 * parameter: maximun number for the randon number to return
 * returns: random number between 0 and parameter max_posible
*/
int generate_solution_position(int max_posible)
{
	/* assign to each virtual machine a random generated physical machine from 0 to the maximum possible */
	/* integer from 0 to max_posible*/
	return rand() % (max_posible);

}
