Wind Farm Simulation
OVERVIEW

This project simulates the energy production of a wind farm based on wind conditions, turbine status, and farm size. The simulation covers daily operations such as energy production and turbine maintenance in case of breakdowns due to strong winds.
Features

    Wind Simulation: Wind conditions vary daily based on a transition matrix.
    Energy Production: Turbines produce energy depending on the wind speed (low, moderate, or strong).
    Maintenance System: Turbines can break down in strong winds and require maintenance, which affects energy production and incurs costs.
    Configurable Parameters: Wind transition probabilities, breakdown likelihood, production rates, and farm size can be customized.

PARAMETERS

    Wind Transition Matrix: Controls daily wind speed changes.
    Failure Probability: Set probability p for turbine breakdowns during strong winds.
    Energy Production Levels: Define energy output for each wind condition (none, moderate, strong).
    Maintenance: Customize duration (3 days by default) and costs (€150,000 per maintenance).

USAGE

    Launch the simulation.
    Customize the parameters through the provided menu options.
    View detailed statistics such as:
        Total energy produced.
        Maintenance costs.
        Maximum and minimum daily production.

REQUIREMENTS

    C compiler.
