"""
This examples demonstrates tuning the paramaters of an external 
simulation by use of ExternalProcessProblem.

Each fitness evaluation launches an external command that you provide,
and sends a phenome to it as a CSV on stdin.  The external command
returns a real number on its stdout, which LEAP reads as the fitness
value.
"""
import logging
import os
import sys

from matplotlib import pyplot as plt

from leap_ec import leap_logger_name, test_env_var
from leap_ec import ops, probe
from leap_ec.algorithm import generational_ea
from leap_ec.problem import ExternalProcessProblem
from leap_ec.real_rep.initializers import create_real_vector
from leap_ec.real_rep.ops import mutate_gaussian
from leap_ec.representation import Representation


if __name__ == '__main__':
    ##############################
    # Parameters
    ##############################
    plots = True
    pop_size = 20
    num_genes = 4
    generations = 50
    max_individuals_per_chunk = 20
    
    ##############################
    # Problem
    # Here we set up the external problem.  It will launch
    # the given command as a subprocess and pipe phenomes into
    # the subprocess's stdin.
    problem = ExternalProcessProblem(command='./build/TuneFiringRates', maximize=True)

    # Uncomment these lines to see logs of what genomes and fitness values are sent to your external process.
    # This is useful for debugging a simulation.
    logging.getLogger().addHandler(logging.StreamHandler())  # Log to stderr
    logging.getLogger(leap_logger_name).setLevel(logging.DEBUG) # Log debug messages

    ##############################
    # Visualization probes
    ##############################
    # We set up visualizations in advance so that we can arrange them nicely
    plt.figure(figsize=(10, 5))
    plt.subplot(121)
    pop_probe = probe.CartesianPhenotypePlotProbe(xlim=[0,1], ylim=[0,1],
                                                  pad=[0.1, 0.1], ax=plt.gca(),
                                                  contours=problem, granularity=0.15)
    plt.subplot(122)
    fit_probe = probe.FitnessPlotProbe(ylim=[0, 0.1], ax=plt.gca())
    viz_probes = [pop_probe, fit_probe]

    ##############################
    # The Evolutionarly Algorithm
    ##############################
    ea = generational_ea(max_generations=generations, pop_size=pop_size,
                            problem=problem,  # Fitness function
                            
                            # By default, the initial population would be evaluated one-at-a-time.
                            # Passing group_evaluate into init_evaluate evaluates the population in batches.
                            init_evaluate=ops.grouped_evaluate(problem=problem, max_individuals_per_chunk=max_individuals_per_chunk),

                            # Representation
                            representation=Representation(
                                # Initialize a population of integer-vector genomes
                                initialize=create_real_vector(
                                    bounds=[[0.0005, 0.5]] * num_genes)
                            ),

                            # Operator pipeline
                            pipeline=[
                                ops.tournament_selection(k=2),
                                ops.clone,  # Copying individuals before we change them, just to be safe
                                mutate_gaussian(std=0.5, hard_bounds=[[0.0005, 0.5]]*num_genes,
                                                expected_num_mutations=1),
                                ops.pool(size=pop_size),
                                # Here again, we use grouped_evaluate to send chunks of individuals to the ExternalProcessProblem.
                                ops.grouped_evaluate(max_individuals_per_chunk=max_individuals_per_chunk),
                                # Print fitness statistics to stdout at each genration
                                probe.FitnessStatsCSVProbe(stream=sys.stdout)
                            ] + (viz_probes if plots else [])
                        )

    best_inds = list(ea)

    if plots:
        # If we're not in test-harness mode, block until the user closes the app
        if os.environ.get(test_env_var, False) != 'True':
            plt.show()
            
        plt.close('all')
