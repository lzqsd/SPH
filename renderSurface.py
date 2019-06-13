import glob
import os.path as osp
import xml.etree.ElementTree as et
from xml.dom import minidom
import os

dataRoot = 'outputNoDivergence'
objFiles = glob.glob(osp.join(dataRoot, '*.obj') )
frameNum = len(objFiles )

# Rendering parameters
isFluid = False
imWidth = 480
imHeight = 360
sampleCount = 1024
fovValue = 64.0
envmapName = '../env.hdr'

renderProgram = '/media/jonathan/program/SceneAR/Code/optixRendererForSUNCG/src/bin/optixSUNCG'
mitsubaProgra = '/media/jonathan/program/mitsuba/build/release/mitsuba/mitsuba'

for n in range(170, frameNum +1):
    print('Frame %d/%d' % (n, frameNum) )

    root = et.Element('scene')
    root.set('version', '0.5.0')
    integrator = et.SubElement(root, 'integrator')
    integrator.set('type', 'path')

    shape = et.SubElement(root, 'shape' )
    shape.set('id', '{0}_object'.format(n) )
    shape.set('type', 'obj' )
    stringF = et.SubElement(shape, 'string' )
    stringF.set('name', 'filename' )
    stringF.set('value', '%08d.obj' % n )

    bsdf = et.SubElement(shape, 'bsdf' )
    bsdf.set('id', 'mat0')
    if isFluid == True:
        bsdf.set('type', 'dielectric')
        str1 = et.SubElement(bsdf, 'string')
        str1.set('name', 'intIOR')
        str1.set('value', 'water')
        str2 = et.SubElement(bsdf, 'string')
        str2.set('name', 'extIOR')
        str2.set('value', 'air')
    else:
        bsdf.set('type', 'microfacet')
        albedo = et.SubElement(bsdf, 'rgb')
        albedo.set('name', 'albedo')
        albedo.set('value', '%.3f %.3f %.3f' % (0.8, 0.8, 0.8) )

        rough = et.SubElement(bsdf, 'float')
        rough.set('name', 'roughness')
        rough.set('value', '%.3f' % (0.2) )

    camera = et.SubElement(root, 'sensor')
    camera.set('type', 'perspective')
    fov = et.SubElement(camera, 'float')
    fov.set('name', 'fov')
    fov.set('value', '%.4f' % (fovValue) )
    fovAxis = et.SubElement(camera, 'string')
    fovAxis.set('name', 'fovAxis')
    fovAxis.set('value', 'x')
    transform = et.SubElement(camera, 'transform')
    transform.set('name', 'toWorld')
    lookAt = et.SubElement(transform, 'lookAt')
    lookAt.set('origin', '-5.5 1.5 0.0')
    lookAt.set('target', '0 0 0')
    lookAt.set('up', '0 1.0 0')
    film = et.SubElement(camera, 'film')
    film.set('type', 'ldrfilm')
    width = et.SubElement(film, 'integer')
    width.set('name', 'width')
    width.set('value', '%d' % (imWidth) )
    height = et.SubElement(film, 'integer')
    height.set('name', 'height')
    height.set('value', '%d' % (imHeight) )
    sampler = et.SubElement(camera, 'sampler')
    sampler.set('type', 'independent')
    sampleNum = et.SubElement(sampler, 'integer')
    sampleNum.set('name', 'sampleCount')
    sampleNum.set('value', '%d' % (sampleCount) )

    emitter = et.SubElement(root, 'emitter')
    emitter.set('type', 'envmap')
    filename = et.SubElement(emitter, 'string')
    filename.set('name', 'filename')
    filename.set('value', envmapName )
    scale = et.SubElement(emitter, 'float')
    scale.set('name', 'scale')
    scale.set('value', '%.4f' % 1.5 )

    rstring = et.tostring(root, 'utf-8')
    pstring = minidom.parseString(rstring)
    xmlString = pstring.toprettyxml(indent="    ")
    xmlString= xmlString.split('\n')
    xmlString = [x for x in xmlString if len(x.strip()) != 0 ]
    xmlString = '\n'.join(xmlString )

    if isFluid:
        xmlName = '%08d_fluid_mit.xml' % n
        imName = '%08d_fluid_mit.png' % n
        cmd = '%s %s -o %s' % (mitsubaProgra,
                osp.join(dataRoot, xmlName), osp.join(dataRoot, imName) )
    else:
        xmlName = '%08d_fluid.xml' % n
        imName = '%08d_fluid.png' % n
        cmd = '%s -f %s -o %s'  % (renderProgram, osp.join(dataRoot, xmlName), imName )

    with open(osp.join(dataRoot, xmlName ), 'w') as xmlOut:
        xmlOut.write(xmlString )

    os.system(cmd )

