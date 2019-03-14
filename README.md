# The Random Fit Algorithm for Virtual Machine Placement (RFVMP) with Oversubscription

This software makes the Virtual Machines (VM) placement considering the oversubscription of the Physical Machines (PM), following a random approach.

This RFVMP approach was developed to be compared with VMPOS (also available at https://github.com/petroniocg) another approach of VM placement, using the informations of Energy Consuption and a parameter called Dispersion.

The RFVMP receives a Datacenter (DC) text file as input with the resources of the machines (physical and virtual) and generates a positioning of all the VMs in the PMs, applying different levels of commitment (oversubscription). In addition, two different types of VMs are considered, both critical and non-critical.

To the placement, the RFVMP take the VMs in sequence and try to allocate them in the PMs in a random way. When the resources of a random PM (considering the oversubscription applied) is not sufficient, the RFVMP try to allocate the VM randomly in other PM, and so on.

## Operation:

_a)_ The datacenter file, with settings of the PMs, as well as the requirements of the VMs, must be passed (here we have one example in **/input** folder) as input to the RFVMP.

The datacenter file must have the following structure (the text part, which separates PMs from VMs, is fixed and must exist):
<br>
<br>
PHYSICAL MACHINES
<br><br>
12	512	10000	1000<br>
16	1024	10000	1300<br>
10	512	10000	1000<br>
8	1024	10000	1300<br>
<br>
VIRTUAL MACHINES
<br><br>
4	15	80	0<br>
8	30	160	0<br>
2	4	32	1<br>
4	8	80	0<br>
8	15	160	1<br>
4	30	320	0<br>
2	60	640	1<br>
2	15	32	0<br>
<br>
Let's call this datacenter file of **4x8.vmp** that is a datacenter with 4 PMs and 8 VMs. The 4 existing lines in the PMs part indicate the resources values for each of the PMs in datacenter. And the sequence in each line represents, respectively: Number of Processors (in the example of the first line, 12), Memory Quantity (in the example of the first line, 512), Disk Capacity (in the example of the first line, 10000) and Energy Consumption (in the example of the first line, 1000). Each row is associated with the resources available from one PM.

The 4 columns in the lines of the VMs part indicate the possible values for required resources by each of VMs in the datacenter. The sequence of these 4 columns represents, respectively: Number of Virtual Processors (in the example of the first line, 4), Memory Amount (in the example of the first line, 15), Disk Capacity (in the example of the first line, 80) and the flag that indicates if the VM run critical services (1) or not (0).

If you want to try another datacenters configurations try to use the SIMULATOR software, available at https://github.com/petroniocg/simulator, that generates random datacenters with the number of PM and VMs you want.

_b)_ Executing: With the DC file ready, the RFVMP is executed as follows:


```sh
$ ./rfvmp input/4x8.vmp
```

where, **4x8.vmp** was the name given to the file with the datacenter configuration.

_c)_ The program will generate two files in the folder **/results**, the main has name **rfvmp_result** which has many informations about the results and, of course, the placement indication of VMs. The second file called **rfvmp_result_data** has just the Energy Consuption and Dispersion informations. With this example, one of the placement results was like this:

****** Solution P ******<br>
1 3 4 4 2 1 3 2<br>

In this result, the numbers (1, 2, 3 and 4) are the PMs, so from the 4 PMs available the RFVMP placed the VMs in 4 PMs (could be in less). The position of the number, from left to right, indicates which VM is allocated in which PM. So, the VMs 1 and 6 are in PM 1. The VMs 5 and 8 are in PM 2. The VMs 2 and 7 are in PM 3. And the VMs 3 and 4 are in the PM 4.

## Modifications:

If you want to adapt the code to your needs, the compilation command to be used is:

```sh
$ gcc -o rfvmp rfvmp.c initialization.c common.c commitment.c -g -lm
```

The oversubscription is applied on PMs through two constants, CL_CS (*Commitment Level for Critical Services*) and CL_NC (*Commitment Level for No Critical Services*) that are configured in in the *commitment.h* file. The current code is using CL_CS = 1.0 and CL_NC = 2.0.

This means that the resources available in the PMs will be multiplied by 1.0 (CL_CS) when it is hosting **at least one VM configured as with critical services**, and that the resources in the PMs will be multiplied by 2.0 (CL_NC) when is hosting **only VMs with non-critical services**.
