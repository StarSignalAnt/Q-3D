class NodeModule:
    def __init__(self, name="UnnamedNode"):
        self.name = name
        self.enabled = True
        self.position = [0.0, 0.0, 0.0]
        self.rotation = [0.0, 0.0, 0.0]
        self.scale = [1.0, 1.0, 1.0]
        self.children = []
        self.parent = None
        self.initialized = False
        self._cpp_test = 0 
    
    # Core lifecycle methods that nodes should override
    def initialize(self):
        """Called when node is first created/added to scene"""
        self.initialized = True
        print(f"Node '{self.name}' initialized")
    
    def update(self, delta_time):
        """Called every frame - override for game logic"""
        pass
    
    def render(self):
        """Called during render pass - override for custom rendering"""
        pass
    
    def cleanup(self):
        """Called when node is destroyed"""
        print(f"Node '{self.name}' cleaned up")
    
    # Transform methods
    def set_position(self, x, y, z):
        self.position = [x, y, z]
    
    def get_position(self):
        return self.position.copy()
    
    def set_rotation(self, x, y, z):
        self.rotation = [x, y, z]
    
    def get_rotation(self):
        return self.rotation.copy()
    
    def set_scale(self, x, y, z):
        self.scale = [x, y, z]
    
    def get_scale(self):
        return self.scale.copy()
    
    # Hierarchy methods
    def add_child(self, child_node):
        if child_node not in self.children:
            self.children.append(child_node)
            child_node.parent = self
    
    def remove_child(self, child_node):
        if child_node in self.children:
            self.children.remove(child_node)
            child_node.parent = None
    
    def get_children(self):
        return self.children.copy()
    
    def get_parent(self):
        return self.parent
    
    # Utility methods
    def enable(self):
        self.enabled = True
    
    def disable(self):
        self.enabled = False
    
    def is_enabled(self):
        return self.enabled
    
    def get_name(self):
        return self.name
    
    def set_name(self, name):
        self.name = name
    
    # String representation for debugging
    def __str__(self):
        return f"NodeModule(name='{self.name}', pos={self.position}, enabled={self.enabled})"

print("=== [NodeModule] Loaded ===")
