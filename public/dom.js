function q() {
    return new (Function.prototype.bind.apply(_q, [null].concat(Array.prototype.slice.call(arguments))));
}
class _q {
    constructor(val) {
        this.e = [window.document];
        this.query(val);
    }
    query(val) {
        if ('string' === typeof val) {
            if (val.length == 0)
                return this;
            if (val[0] === '<') {
                var m = val.match(/^<([^>]+)\/?>?$/);
                if (!m) throw new Error("Invalid element requested: "+val);
                this.e = [window.document.createElement(m[1])];
            } else {
                var elements = this.e.map((e)=>Array.prototype.slice.apply(e.querySelectorAll(val)));
                this.e = Array.prototype.concat.apply([], elements);
            }
        } else if (val instanceof Element) {
            this.e = [val];
        } else if (val instanceof Array) {
            this.e = val;
        } else {
            throw new Error("Invalid query.");
        }
        return this;
    }
    get(index) {
        return this.e[index];
    }
    eq(index) {
        return q(get(index));
    }
    add(e) {
        this.get(0).appendChild(e instanceof _q ? e.get(0) : e);
    }
    remove() {
        return q(this.e.map((e)=>{
            e.parentNode.removeChild(e);
            return e;
        }));
    }
    children() {
        var children = [];
        this.e.forEach((e)=>{
            var c = e.firstElementChild;
            while (c) {
                children.push(c);
                c = c.nextElementSibling;
            }
        });
        return q(children);
    }
    size() {
        return this.e.length;
    }
    each(fn) {
        var i = 0;
        this.e.forEach((e)=>{
            fn(new q(e), i);
            ++i;
        });
        return this;
    }
    screenOffset() {
        return this.e.map((e)=>{
            var rect = e.getBoundingClientRect();
            return {top:rect.top,
                    left:rect.left,
                    width:rect.width,
                    height:rect.height};
        });
    }
    attr(name, opt_val) {
        if (opt_val === void 0)
            return this.e.map((e)=>e.getAttribute(name));
        if (this.e.length != 1)
            throw new Error("Can't modify attributes on multiple results.");
        this.get(0).setAttribute(name, opt_val);
        return this;
    }
    addClass() {
        var args = Array.prototype.slice.call(arguments);
        this.e.forEach(function(e) {
            e.classList.add.apply(e.classList, args);
        });
        return this;
    }
    css(val) {
        if (val instanceof Object) {
            this.e.forEach((e)=>{
                for (var i in val)
                    e.style[i] = val[i];
            });
            return this;
        } else {
            return this.e.map((e)=>e.style[val]);
        }
    }
    bounds() {
        return this.e.map((e)=>{
            return {top: e.offsetTop,
                    left: e.offsetLeft,
                    width: e.clientWidth,
                    height: e.clientHeight};
        });
    }
}
