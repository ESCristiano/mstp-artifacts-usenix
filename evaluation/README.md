# Reproducing Paper Results

This folder contains the datasets and scripts required to reproduce the figures and tables presented in the paper’s evaluation (**Section 6**). 

The table below maps the paper’s results to the corresponding artifact locations.

| Paper's Result 	| Scripts Location							| Datasets Location							|
| :--- 				| :--- 										| :--- 										|
| **Fig. 6a** 		| `./graphs/scripts/gen_matrix.py` 	        | `./graphs/patterns/06-div.txt`			|
| **Fig. 6b** 		| `./graphs/scripts/gen_matrix.py` 	        | `./graphs/patterns/06-inst_diff.txt`		|
| **Fig. 6c** 		| `./graphs/scripts/gen_matrix.py` 	        | `./graphs/patterns/06-cache.txt`			|
| **Fig. 6d** 		| `./graphs/scripts/gen_matrix.py` 	        | `./graphs/patterns/06-contention.txt`	    |
| **Fig. 7a & 10** 	| `./graphs/scripts/gen_template_matrix.py` | `./graphs/patterns/06-shift_template.txt` |
| **Fig. 7b** 		| `./graphs/scripts/gen_template_matrix.py` | `./graphs/patterns/06-sub_template.txt`	|
| **Draw All Figs.**| `./graphs/1-draw-graphs.sh` 	            | `./graphs/patterns/*.txt`	                |
| **Table 5** 		| No Script.							    | `./mstp_metrics/output.txt`		        |
