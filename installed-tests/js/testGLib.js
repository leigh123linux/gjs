const ByteArray = imports.byteArray;
const GLib = imports.gi.GLib;

describe('GVariant constructor', function () {
    it('constructs a string variant', function () {
        let str_variant = new GLib.Variant('s', 'mystring');
        expect(str_variant.get_string()[0]).toEqual('mystring');
        expect(str_variant.deep_unpack()).toEqual('mystring');
    });

    it('constructs a string variant (backwards compatible API)', function () {
        let str_variant = new GLib.Variant('s', 'mystring');
        let str_variant_old = GLib.Variant.new('s', 'mystring');
        expect(str_variant.equal(str_variant_old)).toBeTruthy();
    });

    it('constructs a struct variant', function () {
        let struct_variant = new GLib.Variant('(sogvau)', [
            'a string',
            '/a/object/path',
            'asig', //nature
            new GLib.Variant('s', 'variant'),
            [ 7, 3 ]
        ]);
        expect(struct_variant.n_children()).toEqual(5);

        let unpacked = struct_variant.deep_unpack();
        expect(unpacked[0]).toEqual('a string');
        expect(unpacked[1]).toEqual('/a/object/path');
        expect(unpacked[2]).toEqual('asig');
        expect(unpacked[3] instanceof GLib.Variant).toBeTruthy();
        expect(unpacked[3].deep_unpack()).toEqual('variant');
        expect(unpacked[4] instanceof Array).toBeTruthy();
        expect(unpacked[4].length).toEqual(2);
    });

    it('constructs a maybe variant', function () {
        let maybe_variant = new GLib.Variant('ms', null);
        expect(maybe_variant.deep_unpack()).toBeNull();

        maybe_variant = new GLib.Variant('ms', 'string');
        expect(maybe_variant.deep_unpack()).toEqual('string');
    });

    it('constructs a byte array variant', function () {
        const byteArray = Uint8Array.from('pizza', c => c.charCodeAt(0));
        const byteArrayVariant = new GLib.Variant('ay', byteArray);
        expect(ByteArray.toString(byteArrayVariant.deep_unpack()))
            .toEqual('pizza');
    });

    it('constructs a byte array variant from a string', function () {
        const byteArrayVariant = new GLib.Variant('ay', 'pizza');
        expect(ByteArray.toString(byteArrayVariant.deep_unpack()))
            .toEqual('pizza');
    });

    it('0-terminates a byte array variant constructed from a string', function () {
        const byteArrayVariant = new GLib.Variant('ay', 'pizza');
        const a = byteArrayVariant.deep_unpack();
        [112, 105, 122, 122, 97, 0].forEach((val, ix) =>
            expect(a[ix]).toEqual(val));
    });

    it('does not 0-terminate a byte array variant constructed from a Uint8Array', function () {
        const byteArray = Uint8Array.from('pizza', c => c.charCodeAt(0));
        const byteArrayVariant = new GLib.Variant('ay', byteArray);
        const a = byteArrayVariant.deep_unpack();
        [112, 105, 122, 122, 97].forEach((val, ix) =>
            expect(a[ix]).toEqual(val));
    });
});
