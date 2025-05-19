import tkinter as tk
from tkinter import simpledialog, filedialog

# Color and value schema
COLOR_SCHEME = {
    0: "white",   # empty
    1: "black",   # wall
    2: "blue",    # player spawn
    3: "red",     # enemy spawn
}
REVERSE_COLOR_SCHEME = {v: k for k, v in COLOR_SCHEME.items()}

CELL_SIZE = 15  # Smaller cell size for large grids

class GridEditor:
    def __init__(self, root, size):
        self.root = root
        self.size = size
        self.cell_size = CELL_SIZE
        self.current_value = 1  # Start with wall
        self.matrix = [[0 for _ in range(size)] for _ in range(size)]
        self.rect_ids = {}

        self.setup_ui()
        self.draw_grid()

    def setup_ui(self):
        top_frame = tk.Frame(self.root)
        top_frame.pack(side=tk.TOP, fill=tk.X)

        for val, color in COLOR_SCHEME.items():
            tk.Button(top_frame, text=color.capitalize(), bg=color,
                      command=lambda v=val: self.set_current_value(v)).pack(side=tk.LEFT)

        tk.Button(top_frame, text="Clear", command=self.clear_grid).pack(side=tk.LEFT)
        tk.Button(top_frame, text="Générer", command=self.export_matrix).pack(side=tk.LEFT)

        self.canvas = tk.Canvas(self.root, bg="grey",
                                width=800, height=600,
                                scrollregion=(0, 0, self.size * self.cell_size, self.size * self.cell_size))
        self.canvas.pack(fill=tk.BOTH, expand=True)

        self.h_scroll = tk.Scrollbar(self.root, orient=tk.HORIZONTAL, command=self.canvas.xview)
        self.h_scroll.pack(side=tk.BOTTOM, fill=tk.X)
        self.v_scroll = tk.Scrollbar(self.root, orient=tk.VERTICAL, command=self.canvas.yview)
        self.v_scroll.pack(side=tk.RIGHT, fill=tk.Y)

        self.canvas.configure(xscrollcommand=self.h_scroll.set, yscrollcommand=self.v_scroll.set)

        self.canvas.bind("<Button-1>", self.on_canvas_click)
        self.canvas.bind("<B1-Motion>", self.on_canvas_click)  # drag to paint

    def set_current_value(self, val):
        self.current_value = val

    def draw_grid(self):
        for i in range(self.size):
            for j in range(self.size):
                x0 = j * self.cell_size
                y0 = i * self.cell_size
                x1 = x0 + self.cell_size
                y1 = y0 + self.cell_size
                rect = self.canvas.create_rectangle(x0, y0, x1, y1,
                                                    fill=COLOR_SCHEME[0], outline="lightgrey")
                self.rect_ids[(i, j)] = rect

    def on_canvas_click(self, event):
        x = self.canvas.canvasx(event.x)
        y = self.canvas.canvasy(event.y)

        col = int(x // self.cell_size)
        row = int(y // self.cell_size)

        if 0 <= row < self.size and 0 <= col < self.size:
            self.matrix[row][col] = self.current_value
            rect_id = self.rect_ids[(row, col)]
            self.canvas.itemconfig(rect_id, fill=COLOR_SCHEME[self.current_value])

    def clear_grid(self):
        for (i, j), rect_id in self.rect_ids.items():
            self.matrix[i][j] = 0
            self.canvas.itemconfig(rect_id, fill=COLOR_SCHEME[0])

    def export_matrix(self):
        filename = filedialog.asksaveasfilename(defaultextension=".txt",
                                                filetypes=[("Text files", "*.txt")])
        if filename:
            with open(filename, 'w') as f:
                f.write("int** level = new int*[" + str(self.size) + "] {\n")
                for i, row in enumerate(self.matrix):
                    line = "    new int[" + str(self.size) + "] {" + ", ".join(map(str, row)) + "}"
                    if i < self.size - 1:
                        line += ","
                    f.write(line + "\n")
                f.write("};\n")

if __name__ == "__main__":
    root = tk.Tk()
    root.title("Optimized Grid Level Editor")

    n = simpledialog.askinteger("Taille de la grille", "Entrez la taille n de la grille (nxn) :", minvalue=2, maxvalue=1000)
    if n:
        app = GridEditor(root, n)
        root.mainloop()
