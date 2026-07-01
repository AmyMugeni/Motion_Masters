<div align="center">

<img src="images/university-logo.png" alt="Strathmore University Logo" width="190" />

# Motion Masters Project ICS 4A

**Strathmore University**

Bachelor Of Science in Informatics and Computer Science (BICS)

Embedded Systems and Internet of Things (IOT)

ICS 4A

Deliverable 2

Group: Motion Masters

<img src="images/group_photo_deliverable_2.jpeg" alt="Motion Masters Group Photo" width="720" />

</div>

---

## Table of Contents

- [Question 1a](#question-1a)
- [Question 1b](#question-1b)
- [Question 1c](#question-1c)
- [Challenges Faced](#challenges-faced-across-simulations-b--c)

---

## Question 1a

### The following section presents the physical & simulation implementation of 1a:

### Physical Implementation

<p align="center">
  <img src="images/physical_implementation_1a_deliverable_2.jpeg" alt="Physical Implementation 1A" width="550" />
</p>

<p align="center">
<em>Figure 1: Physical Implementation (1 ESP32S connected to 1 MQ-5, 1 DHT22 and 1 LCD).</em>
</p>

---

### LCD Output

<p align="center">
  <img src="images/lcd_output_1a.png" alt="LCD Output 1A" width="450" />
</p>

<p align="center">
<em>Figure 2: LCD Output.</em>
</p>

---

### IDE Output

<p align="center">
  <img src="images/serial_monitor_1a.jpeg" alt="IDE Output 1A" width="700" />
</p>

<p align="center">
<em>Figure 3: IDE Output.</em>
</p>

---

### Wokwi Simulation

<p align="center">
  <img src="images/wokwi_simulation_1a.png" alt="Simulation 1A" width="700" />
</p>

<p align="center">
<em>Figure 4: Wokwi Simulation.</em>
</p>

### Link to the Wokwi Simulation

<p align="center">
<a href="https://wokwi.com/projects/467656007930712065">Simulation 1A</a>
</p>

---

## Question 1b

### The following section presents the physical & simulation implementation of 1b:

### Physical Implementation

<p align="center">
  <img src="images/physical_1b.jpeg" alt="Implementation 1B" width="550" />
</p>

<p align="center">
<em>Figure 5: Physical Implementation ( 1 ESP32S connected to 1 MQ-5 interfaced directly with another ESP32S connected 
to 1 DHT22).</em>
</p>

---

### IDE Output

<p align="center">
  <img src="images/serial_monitor_1b.jpeg" alt="IDE Output 1B" width="700" />
</p>

<p align="center">
<em>Figure 6: IDE Output.</em>
</p>

---

### Wokwi Simulation

<p align="center">
  <img src="images/wokwi_simulation_1b.png" alt="Simulation 1B" width="700" />
</p>

<p align="center">
<em>Figure 7: Wokwi Simulation.</em>
</p>

### Link to the Wokwi Simulation

<p align="center">
<a href="https://wokwi.com/projects/467884444458313729">Simulation 1B</a>
</p>

---

## Question 1c

### The following section presents the simulation implementation of 1c:

### Wokwi Simulation Implementation

<p align="center">
  <img src="images/implementation_1c.png" alt="Implementation 1C" width="550" />
</p>

<p align="center">
<em>Figure 8: Wokwi Simulation.</em>
</p>

---

### Link to the Wokwi Simulation

<p align="center">
<a href="https://wokwi.com/projects/468337027127865345">Simulation 1C</a>
</p>

---

## Challenges Faced Across Simulations (B & C)

### Wokwi's simulator is designed for one microcontroller at a time.

<p>
Two simulations 
required two ESP32 boards communicating with each other, but Wokwi's environment is 
built around single-board projects. Getting two boards to coexist in one simulation 
required a workaround where both boards were configured within the same project files 
and programmed to figure out their own roles automatically at startup, rather than being 
set up as two completely separate simulations.
</p>

---

### Wiring errors that caused silent failures.

<p>
Across both simulations, certain wiring 
mistakes in the circuit configuration files produced no error messages and no output, 
making them difficult to diagnose. These included connections referencing pins that did 
not exist on the boards and duplicate wires connecting the same points twice. Each issue 
was found through careful manual review of the circuit layout and corrected, after which 
the simulations ran as expected.
</p>

---