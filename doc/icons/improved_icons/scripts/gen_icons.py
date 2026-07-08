"""
Generates the 9 icon SVG masters into ../svg/.

Usage: python3 gen_icons.py
(run from anywhere -- paths are relative to this script's location)
"""
import os
from svghelp import pt, arc_path, tick, badge_ring, write_svg

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, '..', 'svg')
os.makedirs(OUT, exist_ok=True)

FONT = "DejaVu Sans, Arial, sans-serif"

COLORS = {
    '60v_source':     '#20a5ff',
    '10a_source':     '#ffbe00',
    'milliohmmeter':  '#009594',
    'ohmmeter':       '#009d9c',
    'isolation':      '#ff0000',
    'voltmeter':      '#9ceeff',
    'ampmeter':       '#fff600',
    'charge':         '#00ff00',
    'settings':       '#dedede',
}

def gauge(cx, cy, r, color, sw_arc, sw_tick, needle_len, needle_deg, needle_w, hub_r):
    body = ''
    body += f'<path d="{arc_path(cx, cy, r, 210, -30)}" fill="none" stroke="{color}" stroke-width="{sw_arc}" stroke-linecap="round"/>'
    for deg in (210, 150, 90, 30, -30):
        body += tick(cx, cy, r, r - 10, deg, stroke=color, stroke_width=sw_tick, stroke_linecap="round")
    nx, ny = pt(cx, cy, needle_len, needle_deg)
    body += f'<line x1="{cx}" y1="{cy}" x2="{nx:.2f}" y2="{ny:.2f}" stroke="{color}" stroke-width="{needle_w}" stroke-linecap="round"/>'
    body += f'<circle cx="{cx}" cy="{cy}" r="{hub_r}" fill="{color}"/>'
    return body

def meter_icon(color, label, font_size=34):
    body = badge_ring(color)
    body += gauge(128, 150, 50, color, sw_arc=6, sw_tick=5, needle_len=40, needle_deg=55, needle_w=7, hub_r=7)
    body += f'<text x="128" y="92" font-family="{FONT}" font-weight="bold" font-size="{font_size}" fill="{color}" text-anchor="middle">{label}</text>'
    return body

def source_terminal_leads(cx, top_y, bot_y, color, sw=6, r=4):
    body = f'<line x1="{cx}" y1="{top_y}" x2="{cx}" y2="{top_y-18}" stroke="{color}" stroke-width="{sw}" stroke-linecap="round"/>'
    body += f'<circle cx="{cx}" cy="{top_y-18}" r="{r}" fill="{color}"/>'
    body += f'<line x1="{cx}" y1="{bot_y}" x2="{cx}" y2="{bot_y+18}" stroke="{color}" stroke-width="{sw}" stroke-linecap="round"/>'
    body += f'<circle cx="{cx}" cy="{bot_y+18}" r="{r}" fill="{color}"/>'
    return body

def voltage_source_icon(color, label):
    cx, cy, r = 128, 112, 34
    body = badge_ring(color)
    body += f'<circle cx="{cx}" cy="{cy}" r="{r}" fill="none" stroke="{color}" stroke-width="6"/>'
    body += source_terminal_leads(cx, cy - r, cy + r, color)
    body += f'<text x="{cx}" y="{cy-2}" font-family="{FONT}" font-weight="bold" font-size="22" fill="{color}" text-anchor="middle">+</text>'
    body += f'<text x="{cx}" y="{cy+24}" font-family="{FONT}" font-weight="bold" font-size="26" fill="{color}" text-anchor="middle">&#8722;</text>'
    body += f'<text x="{cx}" y="200" font-family="{FONT}" font-weight="bold" font-size="30" fill="{color}" text-anchor="middle">{label}</text>'
    return body

def current_source_icon(color, label):
    cx, cy, r = 128, 112, 34
    body = badge_ring(color)
    body += f'<circle cx="{cx}" cy="{cy}" r="{r}" fill="none" stroke="{color}" stroke-width="6"/>'
    body += source_terminal_leads(cx, cy - r, cy + r, color)
    body += f'<line x1="{cx}" y1="{cy+22}" x2="{cx}" y2="{cy-20}" stroke="{color}" stroke-width="6" stroke-linecap="round"/>'
    body += f'<path d="M {cx-9} {cy-10} L {cx} {cy-24} L {cx+9} {cy-10} Z" fill="{color}"/>'
    body += f'<text x="{cx}" y="200" font-family="{FONT}" font-weight="bold" font-size="30" fill="{color}" text-anchor="middle">{label}</text>'
    return body

def isolation_icon(color):
    body = badge_ring(color)
    bolt = "M 140 62 L 108 132 L 128 132 L 112 190 L 158 118 L 134 118 Z"
    body += f'<path d="{bolt}" fill="{color}"/>'
    for i, (w, y) in enumerate([(46, 208), (30, 218), (16, 226)]):
        body += f'<line x1="{128-w/2}" y1="{y}" x2="{128+w/2}" y2="{y}" stroke="{color}" stroke-width="5" stroke-linecap="round"/>'
    return body

def charge_icon(color):
    body = badge_ring(color)
    body += f'<rect x="72" y="94" width="104" height="68" rx="10" fill="none" stroke="{color}" stroke-width="7"/>'
    body += f'<rect x="176" y="114" width="12" height="28" rx="3" fill="{color}"/>'
    for bx in (84, 104, 124):
        body += f'<rect x="{bx}" y="104" width="12" height="48" fill="{color}" opacity="0.55"/>'
    bolt = "M 140 88 L 118 130 L 132 130 L 122 168 L 156 118 L 138 118 Z"
    body += f'<path d="{bolt}" fill="#000000" stroke="{color}" stroke-width="3" stroke-linejoin="round"/>'
    return body

def settings_icon(color):
    body = badge_ring(color)
    body += f'<line x1="92" y1="176" x2="150" y2="118" stroke="{color}" stroke-width="18" stroke-linecap="round"/>'
    body += f'<path d="{arc_path(168, 100, 26, 55, 280)}" fill="none" stroke="{color}" stroke-width="15" stroke-linecap="round"/>'
    body += f'<circle cx="92" cy="176" r="17" fill="none" stroke="{color}" stroke-width="12"/>'
    body += f'<circle cx="92" cy="176" r="6" fill="#000000"/>'
    return body

ICONS = {
    '60v_source':    voltage_source_icon(COLORS['60v_source'], '60V'),
    '10a_source':    current_source_icon(COLORS['10a_source'], '10A'),
    'milliohmmeter': meter_icon(COLORS['milliohmmeter'], 'mΩ', font_size=28),
    'ohmmeter':      meter_icon(COLORS['ohmmeter'], 'Ω'),
    'isolation':     isolation_icon(COLORS['isolation']),
    'voltmeter':     meter_icon(COLORS['voltmeter'], 'V'),
    'ampmeter':      meter_icon(COLORS['ampmeter'], 'A'),
    'charge':        charge_icon(COLORS['charge']),
    'settings':      settings_icon(COLORS['settings']),
}

if __name__ == '__main__':
    for name, body in ICONS.items():
        write_svg(os.path.join(OUT, f'{name}.svg'), body)
        print('wrote', name)
