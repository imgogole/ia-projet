import tkinter as tk
from tkinter import filedialog, messagebox
from PIL import Image, ImageTk
import json

class RectManager:
    def __init__(self, root):
        self.root = root
        self.root.title("Gestionnaire de rectangles avec zoom")
        self.image = None
        self.photo = None
        self.img_w = self.img_h = 0
        self.rects = []
        self.start = None
        self.zoom = 4
        self.show_rects = tk.BooleanVar(value=True)
        self._build_ui()

    def _build_ui(self):
        menubar = tk.Menu(self.root)
        filemenu = tk.Menu(menubar, tearoff=0)
        filemenu.add_command(label="Ouvrir image…", command=self.open_image)
        menubar.add_cascade(label="Fichier", menu=filemenu)
        self.root.config(menu=menubar)

        main = tk.Frame(self.root)
        main.pack(fill="both", expand=True)

        self.canvas = tk.Canvas(main, bg="white")
        self.canvas.pack(side="left", expand=True)
        self.canvas.bind("<Button-1>", self.on_click)

        ctrl = tk.Frame(main, padx=5)
        ctrl.pack(side="right", fill="y")
        tk.Button(ctrl, text="Zoom +", command=self.zoom_in).pack(fill="x", pady=2)
        tk.Button(ctrl, text="Zoom –", command=self.zoom_out).pack(fill="x", pady=2)
        tk.Checkbutton(ctrl, text="Afficher les rects",
                       variable=self.show_rects,
                       command=self.toggle_rects).pack(anchor="w", pady=5)
        tk.Button(ctrl, text="Charger rects…", command=self.load_rects).pack(fill="x", pady=5)
        tk.Button(ctrl, text="Sauvegarder rects…", command=self.save_rects).pack(fill="x", pady=5)

        self.list_frame = tk.Frame(ctrl)
        self.list_frame.pack(fill="both", expand=True)

    def open_image(self):
        path = filedialog.askopenfilename(filetypes=[("Images", "*.png *.jpg *.jpeg")])
        if not path:
            return
        img = Image.open(path)
        if img.size != (100, 100):
            messagebox.showerror("Erreur", "L'image doit faire exactement 100×100 px.")
            return
        self.image = img
        self.img_w, self.img_h = img.size
        self.zoom = 4
        for r in list(self.rects):
            self._delete_rect(r)
        self.apply_zoom()

    def load_rects(self):
        if self.image is None:
            messagebox.showwarning("Aucune image", "Chargez d'abord une image 100×100px avant de charger des rectangles.")
            return
        path = filedialog.askopenfilename(filetypes=[("JSON", "*.json")])
        if not path:
            return
        try:
            with open(path, 'r') as f:
                data = json.load(f)
        except Exception as e:
            messagebox.showerror("Erreur JSON", f"Impossible de lire le fichier JSON:\n{e}")
            return
        if not isinstance(data, list):
            messagebox.showerror("JSON invalide", "Le fichier JSON doit contenir une liste de rectangles.")
            return
        # vider anciens rectangles
        for r in list(self.rects):
            self._delete_rect(r)
        # redessiner image et grille
        self.apply_zoom()
        # ajouter chaque rectangle
        for item in data:
            try:
                x0 = item['x']
                y0 = item['y']
                width  = item['width']
                height = item['height']
            except KeyError:
                continue
            self._add_rect(x0, y0, width, height)

    def apply_zoom(self):
        if not self.image:
            return
        z = self.zoom
        scaled = self.image.resize((self.img_w*z, self.img_h*z), Image.NEAREST)
        self.photo = ImageTk.PhotoImage(scaled)
        self.canvas.config(width=self.img_w*z, height=self.img_h*z)
        self.canvas.delete("all")
        self.canvas.create_image(0, 0, anchor="nw", image=self.photo)
        self._draw_grid()
        state = "normal" if self.show_rects.get() else "hidden"
        for r in self.rects:
            x0, y0 = r["x"]*z, r["y"]*z
            x1, y1 = (r["x"]+r["w"])*z, (r["y"]+r["h"])*z
            rid = self.canvas.create_rectangle(x0, y0, x1, y1,
                                               fill="black", stipple="gray50", state=state)
            r["id"] = rid

    def _draw_grid(self):
        z = self.zoom
        for i in range(self.img_w+1):
            x = i*z
            self.canvas.create_line(x, 0, x, self.img_h*z, fill="#ddd")
        for j in range(self.img_h+1):
            y = j*z
            self.canvas.create_line(0, y, self.img_w*z, y, fill="#ddd")

    def on_click(self, event):
        if self.image is None:
            return
        z = self.zoom
        img_x = event.x // z
        img_y = event.y // z
        if not (0 <= img_x < self.img_w and 0 <= img_y < self.img_h):
            return
        if not self.start:
            self.start = (img_x, img_y)
        else:
            x1, y1 = self.start
            x2, y2 = img_x, img_y
            x0, y0 = min(x1, x2), min(y1, y2)
            w, h = abs(x2 - x1) + 1, abs(y2 - y1) + 1
            if w > 0 and h > 0:
                self._add_rect(x0, y0, w, h)
            self.start = None

    def _add_rect(self, x, y, w, h):
        z = self.zoom
        rid = self.canvas.create_rectangle(x*z, y*z, (x+w)*z, (y+h)*z,
                                           fill="black", stipple="gray50")
        entry = {"x": x, "y": y, "w": w, "h": h, "id": rid, "frame": None}

        frame = tk.Frame(self.list_frame)
        frame.pack(fill="x", pady=2)
        lbl = tk.Label(frame, text=f"({x},{y}) {w}×{h}")
        lbl.pack(side="left")
        btn = tk.Button(frame, text="Suppr", command=lambda e=entry: self._delete_rect(e))
        btn.pack(side="right")

        entry["frame"] = frame
        self.rects.append(entry)

    def _delete_rect(self, entry):
        self.canvas.delete(entry["id"])
        entry["frame"].destroy()
        self.rects.remove(entry)

    def toggle_rects(self):
        state = "normal" if self.show_rects.get() else "hidden"
        for r in self.rects:
            self.canvas.itemconfigure(r["id"], state=state)

    def zoom_in(self):
        if self.image and self.zoom < 20:
            self.zoom += 1
            self.apply_zoom()

    def zoom_out(self):
        if self.image and self.zoom > 1:
            self.zoom -= 1
            self.apply_zoom()

    def save_rects(self):
        if not self.rects:
            messagebox.showinfo("Info", "Aucun rectangle à sauvegarder.")
            return
        path = filedialog.asksaveasfilename(defaultextension=".json",
                                            filetypes=[("JSON", "*.json")])
        if not path:
            return
        data = [{"x": r["x"], "y": r["y"], "width": r["w"], "height": r["h"]}
                for r in self.rects]
        with open(path, "w") as f:
            json.dump(data, f, indent=2)
        messagebox.showinfo("Succès", f"Rectangles sauvegardés dans :\n{path}")

if __name__ == "__main__":
    root = tk.Tk()
    app = RectManager(root)
    root.mainloop()
