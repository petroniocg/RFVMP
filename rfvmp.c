/* 
 * rfvmp.c: Random Fit Algorithm for Virtual Machine Placement
 * Date: 14-09-2018
 * Author: Petrônio Carlos Bezerra (petroniocg@ifpb.edu.br)
 * 
 * This code is based in imavmp.c developed by Fabio Lopez Pires, as follows
 * imavmp.c: Interactive Memetic Algorithm for Virtual Machine Placement (IMAVMP)
 * Date: 17-11-2014
 * Author: Fabio Lopez Pires (flopezpires@gmail.com)
 * Corresponding Conference Paper: A Many-Objective Optimization Framework for Virtualized Datacenters
 * Available at: https://github.com/flopezpires
*/


/* include libraries */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* include own headers */
#include "common.h"
#include "initialization.h"
#include "commitment.h"
//#include "allocation.h"

#define CRITICAL_SERVICES 1 //Indicates if the VM run Critical Services

#define NUMBER_OF_PARAMETERS_PM 4
#define NUMBER_OF_PARAMETERS_VM 4

//These variables will be used to compare RFVMP to VMPOS
int number_vms_cs = 0;
int number_alocated_vms = 0;

/* main: Random Fit Algorithm for Virtual Machine Placement
 * parameter: path to the datacenter infrastructure file
 * returns: exit state
*/
int main (int argc, char *argv[]) {

	FILE *rfvmp_result;
	FILE *rfvmp_result_data;

    /* parameters verification */
	if (argc == 1)
	{
		/* wrong parameters */
		rfvmp_result = fopen("results/rfvmp_result","a");

		fprintf(rfvmp_result,"\n[ERROR] usage: %s path_to_datacenter_file.\nOr, [ERROR]: File not found. Check the writing, please.\n\n", argv[0]);

		fclose(rfvmp_result);		
		/* finish him */
		return 1;
	}

    /* good parameters */	
	else
	{
		/* RF Algorithm previous stuff */

		/* get the number of physical machines and virtual machines from the datacenter infrastructure file (argv[1]) */
		int h_size = get_h_size(argv[1]); // ** Num of PMs **
		int v_size = get_v_size(argv[1]); // ** Num of VMs **

		rfvmp_result = fopen("results/rfvmp_result","a");
		fprintf(rfvmp_result,"===>  THIS IS THE RANDOM FIT VMP  <===\n");
		fprintf(rfvmp_result,"\nDATACENTER:\nNum of PMs: h_size = %d,\nNum of VMs: v_size = %d\n",h_size,v_size);
		fclose(rfvmp_result);

		/* load physical machines resources and virtual machines requirements from the datacenter infrastructure file */
		int **H = load_H(h_size, argv[1]);
		printf("\nH LOADED SUCCESSFULLY\n");
		print_int_matrix(H, h_size, NUMBER_OF_PARAMETERS_PM);

		int **V = load_V(v_size, argv[1], CRITICAL_SERVICES);
		printf("\nV LOADED SUCCESSFULLY\n");
		print_int_matrix(V, v_size, NUMBER_OF_PARAMETERS_VM);

		/* seed for rand() */
		srand((unsigned int) time(NULL));

		/* RF Algorithm starts here */

		/*******************************************/
		/* 01: Initialize Solution P with 0	       */
		/*******************************************/
	
		int *P = initialization(h_size, v_size, V, CRITICAL_SERVICES);

		//printf("\nP INITIALIZED SUCCESSFULLY\n");
		//print_int_array_line(P, v_size);

		/* CM: Commitment Matrix. Has the level of commitment for each PM in each individual */
		/* Here, in RF algorithm, the commitment is used just to calculate the value of dispersion objective */
		/* It will just be used to compare RFVMP to VMPOS (Another VM placement approach available at: https://github.com/petroniocg) */
		float *CM;
		CM = calculates_commitment(P, h_size, v_size, V, CRITICAL_SERVICES);

		//printf("\n====> COMMITMENT MATRIX INICIAL  <=====\n");
		//print_float_array(CM, h_size);

		/* Additional task: load the utilization of physical machines of all individuals/solutions */
		int **utilization_P = load_utilization(P, H, V, h_size, v_size);

		//printf("\nP UTILIZATION INITIALIZED SUCCESSFULLY");
		//printf("\nUTILIZATION OF PMs:\n");
		//print_int_matrix(utilization_P, h_size, 3);

		/************************************************/
		/* 02: Alocate the VMs to PMs and update P */
		/************************************************/
		P = allocate(P, utilization_P, CM, H, V, h_size, v_size, CRITICAL_SERVICES);

		//printf("\nP ALLOCATED SUCCESSFULLY\n");
		//print_int_array_line(P, v_size);
		print_int_array_line_file(P, v_size);

		/* Additional task: calculate the cost of each objective function for each solution */
		float *objectives_functions_P = load_objectives(P, utilization_P, CM, H, V, h_size, v_size, CRITICAL_SERVICES);

		//printf("\nP_0 OBJECTIVE FUNCTIONS VALUES CALCULATED SUCCESSFULLY\n");
		//print_float_array(objectives_functions_P, 2);

		rfvmp_result = fopen("results/rfvmp_result","a");
		rfvmp_result_data = fopen("results/rfvmp_result_data","a+");
		fprintf(rfvmp_result,"Number of Alocated VMs: %d \n", number_alocated_vms);
		fprintf(rfvmp_result,"\nCOSTS:	Energy Cons.: %g\tDispersion Level.: %g\n",objectives_functions_P[0], objectives_functions_P[1]);
		fprintf(rfvmp_result_data,"%g\t%g\n",objectives_functions_P[0], objectives_functions_P[1]);
		fprintf(rfvmp_result,"Commitment of the PMs:\n");
		fclose(rfvmp_result);
		fclose(rfvmp_result_data);

		print_float_array_line_file(CM, h_size);

		/* finish him */
		return 0;
	}
}

