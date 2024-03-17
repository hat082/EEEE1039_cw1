from graphviz import Digraph

# Create a Digraph object
dot = Digraph()

# Add nodes and edges to the graph
dot.node('A', 'Start')
dot.edge('A', 'B', label='Link text')
dot.node('B', 'Decision')
dot.edge('B', 'C', label='Yes')
dot.edge('B', 'D', label='No')
dot.node('C', 'Result 1')
dot.node('D', 'Result 2')
dot.edge('C', 'E', label='End')
dot.edge('D', 'E')

# display the graph
dot.render('diagram', view=True)
