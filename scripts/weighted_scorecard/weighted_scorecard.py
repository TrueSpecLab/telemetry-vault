import matplotlib.pyplot as plt
import matplotlib.font_manager as fm

# ==========================================
# PART 1: THE CONFIGURATION & RAW DATA
# ==========================================
COLOR_BG        = '#00000000'    # Transparent background
COLOR_HEADER    = '#ffffff2A'    # Semi-transparent White
COLOR_TEXT      = '#F2F3F4'      # Off-White
COLOR_BORDER    = '#6B7077'      # Muted Gray
COLOR_RED       = '#C62828'      # Failure Red
COLOR_GREEN     = '#2E7D32'      # Success Green
COLOR_NOTE      = '#F9A825'      # Gold for the Winner

# --- FONT CONFIGURATION ---
# 1. The Default Font (Everything except the title)
FONT_FAMILY     = 'IBM Plex Mono'  # Change this to your preferred font (must be installed on your system)
FONT_WEIGHT     = 'medium' # 'light', 'normal', 'medium', 'semibold', 'bold', 'heavy'   

# 2. The Title Font (Custom Bold File)
MY_PATH         = os.path.dirname(os.path.abspath(__file__)) # Get the directory of the current script

# Replace with your actual path to the bold font file you want to use for the title
PATH_BOLD       = MY_PATH + '\\fonts\\IBMPlexSans-Medium.ttf'

try:
    # Load the specific bold font file for the TITLE only
    prop_bold = fm.FontProperties(fname=PATH_BOLD)
    print(f"Success: Loaded title font from {PATH_BOLD}")
except:
    print("WARNING: Custom Title Font not found. Using default.")
    prop_bold = None

# --- APPLY GLOBAL FONT SETTINGS ---
# This sets 'IBM Plex Mono' as the font for the table, labels, and notes
plt.rcParams.update({
    'font.family': FONT_FAMILY,
    'font.weight': FONT_WEIGHT,
    'axes.labelweight': FONT_WEIGHT,
    'font.size': 20  # Optional: sets a base font size
})

# --- TEST CONFIGURATION ---
# 1. Constants (The "Rules" of the Test)
FAILURE_THRESHOLD_V = 0.20  # The voltage drop limit (0.20V) before we consider it a failure.
                            # We use this to normalize the score. 0.20V drop = Score of 0.

# 2. Weights (How important is each metric?)
WEIGHTS = {
    'stability': 0.40, # 40% - Most important: Does it work WHEN cold?
    'recovery':  0.30, # 30% - Does it survive PERMANENTLY?
    'overhead':  0.20, # 20% - Does it have high voltage potential?
    'value':     0.10  # 10% - Is it worth the money?
}

# 3. Raw Battery Data (Input your Multimeter Readings here)
# Format: [Name, V_base (Start), V_test (Under Load @ -18C), V_thaw (After Warming), Value_Score (Subjective 0-10)]
batteries = [
    {
        "name": "LITHIUM\n(Li-FeS2)",
        "v_base": 1.42,  # Fresh out of the pack
        "v_test": 1.38,  # Small drop under load
        "v_thaw": 1.38,  # Slight loss
        "val_score": 4.0 # Expensive!
    },
    {
        "name": "NiMH\n(Recharge)",
        "v_base": 1.21,  # Fresh out of the pack
        "v_test": 1.20,  # Rock solid stability
        "v_thaw": 1.21,  # No permanent loss
        "val_score": 9.5 # Great value (reusable)
    },
    {
        "name": "ALKALINE\n(Zn/MnO2)",
        "v_base": 1.43,  # Fresh out of the pack
        "v_test": 1.38,  # Slight sag in cold
        "v_thaw": 1.41,  # Did not recover that lost voltage
        "val_score": 8.0 # Cheap and decent
    },
    {
        "name": "C-ZINC\n(Heavy Duty)",
        "v_base": 1.39,  # Starting voltage (from your thumbnail!)
        "v_test": 1.27,  # Big drop (from your thumbnail!)
        "v_thaw": 1.36,  # Some recovery, but damaged
        "val_score": 9.0 # Very cheap
    }
]

# ==========================================
# PART 2: THE CALCULATION ENGINE
# ==========================================
def calculate_scores(battery_list):
    # Find the highest starting voltage in the whole group to normalize "Overhead"
    # Logic: The battery with the highest voltage gets a 10/10 for Overhead.
    max_v_base = max(bat['v_base'] for bat in battery_list)

    # Process each battery
    processed_data = []

    for bat in battery_list:
        # --- Metric 1: Stability (Delta under Load) ---
        # Formula: 10 - ((Drop / Threshold) * 10)
        # Example: If drop is 0.01V and limit is 0.20V, you lost 5% of your score -> 9.5
        delta = bat['v_base'] - bat['v_test']
        score_stability = 10 - ((delta / FAILURE_THRESHOLD_V) * 10)
        score_stability = max(0, min(10, score_stability)) # Clamp between 0 and 10

        # --- Metric 2: Recovery (Permanent Loss) ---
        # Formula: 10 - ((Permanent Loss / Threshold) * 10)
        # Measures if the chemistry was permanently damaged by the freeze.
        loss = bat['v_base'] - bat['v_thaw']
        score_recovery = 10 - ((loss / FAILURE_THRESHOLD_V) * 10)
        score_recovery = max(0, min(10, score_recovery))

        # --- Metric 3: Overhead (Voltage Potential) ---
        # Formula: (This Battery's Voltage / Highest Voltage in Group) * 10
        # A 1.2V battery is inherently "weaker" than a 1.5V battery for high-voltage tools.
        score_overhead = (bat['v_base'] / max_v_base) * 10
        
        # --- Metric 4: Value (Subjective Input) ---
        score_value = bat['val_score']

        # --- FINAL INDEX CALCULATION ---
        # Sum of (Score * Weight)
        final_index = (
            (score_stability * WEIGHTS['stability']) +
            (score_recovery * WEIGHTS['recovery']) +
            (score_overhead * WEIGHTS['overhead']) +
            (score_value * WEIGHTS['value'])
        )

        # Append formatted data for the table
        processed_data.append({
            "name": bat['name'],
            "s1": score_stability,
            "s2": score_recovery,
            "s3": score_overhead,
            "s4": score_value,
            "final": final_index
        })
    
    return processed_data

# ==========================================
# PART 3: DRAWING THE TABLE (Matplotlib)
# ==========================================
def draw_scorecard(results):
    # Prepare Data for Table
    # Row 1: Stability, Row 2: Recovery, Row 3: Overhead, Row 4: Value, Row 5: Index
    # We need to pivot the data from "List of Batteries" to "List of Rows"
    
    columns = ["METRIC", "WEIGHT"] + [bat['name'] for bat in results]
    
    cell_data = [
        # Row 1
        ["Cold Stability($\\Delta$)", "40%"] + [f"{bat['s1']:.1f}" for bat in results],
        # Row 2
        ["Chemical Recovery", "30%"] + [f"{bat['s2']:.1f}" for bat in results],
        # Row 3
        ["Voltage Overhead", "20%"] + [f"{bat['s3']:.1f}" for bat in results],
        # Row 4
        ["Value Prop (ROI)", "10%"] + [f"{bat['s4']:.1f}" for bat in results],
        # Row 5 (Final)
        ["INDEX", "100%"] + [f"{bat['final']:.1f}" for bat in results]
    ]

    # Create Figure and Axis
    fig, ax = plt.subplots(figsize=(21, 9))
    fig.patch.set_facecolor(COLOR_BG) # Transparent background
    ax.set_axis_off()

    # Create Table
    table = ax.table(
        cellText=cell_data,
        colLabels=columns,
        loc='center',
        cellLoc='center',
        bbox=[0.05, 0.1, 0.97, 0.8] # Margins [left, bottom, width, height]
    )

    # Turn off auto-scaling
    table.auto_set_font_size(False)

    # Styling Loop
    for (row, col), cell in table.get_celld().items():
        cell.set_edgecolor(COLOR_BORDER)
        cell.set_linewidth(1.5)

        # Header
        if row == 0:
            cell.set_text_props(color=COLOR_TEXT, weight='bold', size=22)
            cell.set_facecolor(COLOR_HEADER) # Semi-transparent white
            cell.set_height(0.12)

        # Body Rows
        else:
            cell.set_text_props(color=COLOR_TEXT)
            cell.set_facecolor(COLOR_BG) # Transparent background for body cells
            cell.set_height(0.15)
            
            # Increase font size for data columns (not Metric or Weight)
            if row != 0 and row != 5:
                # Weight Column
                if col == 1:
                    cell.set_text_props(size=30)

                # Data Columns
                if col >= 2:
                    cell.set_text_props(size=32)

            # --- CONDITIONAL FORMATTING (The "Smart" Coloring) ---
            # 1. Highlight the FINAL INDEX Row
            if row == 5:
                cell.set_height(0.18)
                cell.set_text_props(weight='bold', size=34)
                
                # Check if this column is a battery score (cols 2,3,4,5)
                if col >= 2:
                    # Set default to trasparent so I can animate later
                    cell.set_text_props(color=COLOR_BG) # Start with invisible text
                    
                    val = float(cell_data[row-1][col]) # Get the score value

                    # GOLD for highest score
                    # round all final scores to 1 decimal for comparison
                    all_scores = [round((r['final']), 1) for r in results]

                    if val == max(all_scores):
                        cell.set_facecolor(COLOR_NOTE) # Highlight the cell with gold background
                        #cell.set_text_props(color='black', weight='bold')
                    # RED for failures (< 7.0)
                    elif val < 7.0:
                        cell.set_text_props(color=COLOR_RED, weight='bold')

            # 2. Highlight Specific Failures/Wins in Metrics
            if row >= 1 and row <= 3 and col >= 2:
                val = float(cell_data[row-1][col])
                if val < 5.0: # Severe failure
                     cell.set_text_props(color=COLOR_RED, weight='bold')
                if val > 9.0: # Exceptional performance
                    cell.set_text_props(color=COLOR_GREEN, weight='bold')

    # --- TITLE WITH CUSTOM FONT ---
    # We use 'fontproperties' here to target ONLY the title with the custom bold font
    plt.title(
        "72-HOUR FREEZE TEST: The TRUESPEC LAB Index", 
        color=COLOR_TEXT, 
        fontproperties=prop_bold,  # <--- THIS IS THE FIX
        fontsize=40,               # Increased size since Space Grotesk is often smaller
        pad=5, 
        loc='center'
    )

    # --- FOOTNOTE WITH DEFAULT FONT ---
    plt.text(
        0.05, 0.05, 
        "test_condition: -18°C saturation // load_profile: 500mA modified torch // delta_limit: 0.20V",
        color=COLOR_BORDER,
        fontsize=22,
        weight='medium' # Uses the default font family (IBM Plex Mono)
    )

    plt.tight_layout()
    plt.savefig('weighted_scorecard.png', dpi=300, facecolor=COLOR_BG) # Save with transparent background
    return fig

# Execute
if __name__ == "__main__":
    results = calculate_scores(batteries)
    draw_scorecard(results)
    #plt.show()
