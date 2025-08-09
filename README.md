<table border="1" cellpadding="5" cellspacing="0">
  <thead>
    <tr>
      <th>Componente</th>
      <th>Pin Arduino</th>
      <th>Pin L298N</th>
      <th>Pin TCRT5000</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Sensor 1 (S1)</td>
      <td>A0</td>
      <td>-</td>
      <td>OUT</td>
    </tr>
    <tr>
      <td>Sensor 2 (S2)</td>
      <td>A1</td>
      <td>-</td>
      <td>OUT</td>
    </tr>
    <tr>
      <td>Sensor 3 (S3)</td>
      <td>A2</td>
      <td>-</td>
      <td>OUT</td>
    </tr>
    <tr>
      <td>Sensor 4 (S4)</td>
      <td>A3</td>
      <td>-</td>
      <td>OUT</td>
    </tr>
    <tr>
      <td>Motor Izq.</td>
      <td>D9 (IN1), D10 (IN2), D5 (ENA)</td>
      <td>IN1, IN2, ENA</td>
      <td>-</td>
    </tr>
    <tr>
      <td>Motor Der.</td>
      <td>D6 (IN3), D7 (IN4), D11 (ENB)</td>
      <td>IN3, IN4, ENB</td>
      <td>-</td>
    </tr>
    <tr>
      <td>VCC Sensores</td>
      <td>5V</td>
      <td>-</td>
      <td>VCC</td>
    </tr>
    <tr>
      <td>GND Sensores</td>
      <td>GND</td>
      <td>-</td>
      <td>GND</td>
    </tr>
    <tr>
      <td>L298N Power</td>
      <td>-</td>
      <td>12V, GND</td>
      <td>-</td>
    </tr>
    <tr>
      <td>Arduino Power</td>
      <td>VIN, GND</td>
      <td>5V, GND (común)</td>
      <td>-</td>
    </tr>
  </tbody>
</table>

Control PID avanzado

Filtro en el término integral:
Se añadió un límite al acumulado de la integral (integralLimit = 50.0). Así, el valor de la integral se mantiene entre -integralLimit y integralLimit usando constrain(). Esto evita que crezca demasiado cuando el carro entra en curvas cerradas o pierde la línea temporalmente, mejorando la estabilidad general.

Ajuste dinámico de Kp y Kd:

Si el error es grande (abs(error) > 1.0), Kp aumenta un 50% (Kp_base * 1.5) para hacer correcciones más rápidas, y Kd se reduce un 20% (Kd_base * 0.8) para minimizar oscilaciones.

Si el error es pequeño, Kp vuelve a su valor original y Kd aumenta un 20% (Kd_base * 1.2) para mejorar la estabilidad en tramos rectos o suaves.

Estos cambios hacen que el carro responda ágilmente en curvas, pero mantenga estabilidad en rectas, lo que es ideal para pistas de competencia.

Estrategia en intersecciones

Memorización de rutas:
Se usa un arreglo pathHistory (máximo 20 registros) para guardar las decisiones tomadas:

L: izquierda

R: derecha

S: recto
Además, hay contadores (leftCount, rightCount, straightCount) para saber cuántas veces se ha tomado cada dirección.

Elección de dirección:
La función chooseDirection() elige la opción con menor cantidad de repeticiones en el historial, dando prioridad a izquierda o derecha si los sensores extremos (digitalValues[0] o digitalValues[3]) detectan disponibilidad. Si no hay preferencia, sigue recto. Esto evita que el carro se quede atrapado en bucles.

Nuevas funciones:
Se añadieron turnLeft() y goStraight() para ejecutar las decisiones tomadas.

Mantenimiento de las funciones originales

El sistema sigue contando con:

Calibración automática.

Seguimiento de líneas negras (continuas o segmentadas).

Manejo de curvas.

Parada automática al perder la línea.

Se mantienen las mismas conexiones y parámetros base (Kp_base, Ki, Kd_base, baseSpeed, maxSpeed), solo que ahora se aplican ajustes dinámicos.

Cómo implementarlo

Conexiones:

Sensores TCRT5000: A0 a A3

L298N: Pines 5, 6, 7, 9, 10, 11

GND común y batería correctamente conectada (12 V para L298N, 5 V/VIN para Arduino).

Calibración:
Durante los primeros 10 s, mueve el carro por la línea negra y el fondo blanco para que los sensores se ajusten.
Si la lectura no es precisa, ajusta sensorThreshold o corrige la altura de los sensores (1–2 cm del suelo).

Ajuste PID:

Valores iniciales recomendados: Kp_base=25, Ki=0.1, Kd_base=15.

Si oscila mucho en curvas, ajusta integralLimit (50.0).

Modifica los factores dinámicos (1.5 para Kp, 0.8/1.2 para Kd) según la pista.

Ajusta baseSpeed (100) y maxSpeed (200) para mayor velocidad en pistas rápidas o menor velocidad en pistas con curvas cerradas.

Pruebas:
Coloca el carro en una pista con líneas negras (continuas o segmentadas), curvas e intersecciones tipo T o Y.
Verifica que:

Siga la línea suavemente.

Tome decisiones correctas en intersecciones evitando repetir caminos.

Se detenga si pierde la línea.
Usa el puerto Serial para depurar (error, Kp, Kd o pathHistory).


///Pasatiempos

<pre style="font-family: monospace; font-size: 14px; line-height: 1.2; white-space: pre-wrap;">
 █████╗ ██████╗ ██╗  ██╗    ██████╗ ███████╗██╗   ██╗
██╔══██╗██╔══██╗██║ ██╔╝    ██╔══██╗██╔════╝██║   ██║
███████║██████╔╝█████╔╝     ██║  ██║█████╗  ██║   ██║
██╔══██║██╔══██╗██╔═██╗     ██║  ██║██╔══╝  ╚██╗ ██╔╝
██║  ██║██║  ██║██║  ██╗    ██████╔╝███████╗ ╚████╔╝ 
╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝    ╚═════╝ ╚══════╝  ╚═══╝  
</pre>

# ✨ Ark Dev

## 🛡️ Licencia de Uso – MIT + Política de Uso y Distribución General

**Copyright (c) 2025 Ark Dev**

Se otorga permiso, sin costo, a cualquier persona que obtenga una copia de este software y de los archivos de documentación asociados (el “Software”), a tratar el Software sin restricciones, incluidos, entre otros, los derechos de usar, copiar, modificar, fusionar, publicar, distribuir, sublicenciar y/o vender copias del Software, y a permitir a las personas a quienes se les proporcione el Software que lo hagan, sujeto a las siguientes condiciones:

> 📝 **Este aviso de copyright y permiso deberá incluirse en todas las copias o partes sustanciales del Software.**

---

## ⚠️ Exención de Responsabilidad

El Software se proporciona "**tal cual**", sin garantía de ningún tipo, expresa o implícita, incluidas, entre otras, las garantías de comercialización, idoneidad para un propósito particular y no infracción.  
Los autores no serán responsables por ningún daño directo o indirecto, pérdida de datos o mal uso que surja del uso del Software.

---

## 📄 Política General de Uso

Este repositorio puede contener diferentes tipos de software:

- 🔓 **Software Libre**: Código abierto bajo Licencia MIT (o similar), reutilizable y modificable sin restricciones.
- 🔒 **Software Privativo**: Código restringido, protegido por derechos de autor. **No se permite su redistribución sin autorización**.
- 🧪 **Software en Fase Beta o Experimental**: Puede contener errores, estar incompleto o no estar disponible para uso comercial.

> Cada proyecto tiene un archivo `LICENSE` que especifica su uso permitido. Asegúrate de revisarlo antes de utilizar el software.

---

## 💿 Copias, Descargas y Falsificación

### ✅ Permitido:
- Descargar copias locales del software.
- Usar o modificar el software bajo licencia libre con atribución.
- Crear forks públicos de los repositorios abiertos.

### ❌ Prohibido:
- Reivindicar la autoría de proyectos sin cambios significativos.
- Redistribuir software privativo o en fase beta sin permiso por escrito.
- Falsificar identidad del autor (Joel Andres / Ark Dev).
- Usar el software para fines ilegales, maliciosos o que infrinjan derechos de terceros.

---

## 🔐 Sobre Software Libre, Privativo y Licencias

Este repositorio puede contener licencias mixtas:

- **MIT**: Libertad de uso, modificación, distribución y sublicencia.
- **Propietaria (Privativa)**: Requiere autorización para cualquier uso comercial o redistribución.
- **Restricciones adicionales**:
  - Algunos módulos pueden incluir dependencias con licencias propias (Apache, GPL, CC, etc.).
  - Las imágenes, sonidos, vídeos o recursos visuales pueden tener licencias distintas.
  - Scripts o herramientas en fase de pruebas no están autorizadas para entornos de producción.

> **Cualquier uso indebido puede constituir una infracción de derechos de autor o uso no autorizado.**

---

## 📢 Nota sobre Software en Pruebas

Parte del contenido puede estar en **fase de desarrollo o prueba (beta)**. Esto implica que:

- Su funcionamiento no está garantizado.
- Está sujeto a cambios sin previo aviso.
- Su uso en producción está desaconsejado sin revisión técnica.

---

## 📬 Contacto Profesional

**Autor:** Joel Andres – *Ark Dev System*  
📅 Agenda una reunión: [https://cal.com/ark-dev](https://cal.com/ark-dev)  
🌐 Sitio Web: [https://arkdev.pages.dev](https://arkdev.pages.dev)  

---

> 💡 *Gracias por visitar este proyecto. Si te parece útil, no olvides dejar una estrella ⭐ y compartirlo.*  
> **Estoy comprometido con el desarrollo ético y responsable del software. Acepto el reto. 😎**


